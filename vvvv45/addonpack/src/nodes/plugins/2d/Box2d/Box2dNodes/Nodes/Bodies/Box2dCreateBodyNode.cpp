#include "StdAfx.h"
#include "Box2dCreateBodyNode.h"
#include "../../Internals/Data/BodyCustomData.h"
#include "../../Internals/Data/ShapeCustomData.h"

namespace VVVV 
{
	namespace Nodes 
	{
		Box2dCreateBodyNode::Box2dCreateBodyNode(void)
		{
			this->mBodies = gcnew BodyDataType();
		}

		void Box2dCreateBodyNode::SetPluginHost(IPluginHost^ Host) 
		{
			this->FHost = Host;

			//World input
			this->FHost->CreateNodeInput("World",TSliceMode::Single,TPinVisibility::True,this->vInWorld);
			this->vInWorld->SetSubType(ArrayUtils::SingleGuidArray(WorldDataType::GUID),WorldDataType::FriendlyName);

			this->FHost->CreateNodeInput("Shapes",TSliceMode::Dynamic,TPinVisibility::True,this->vInShapes);
			this->vInShapes->SetSubType(ArrayUtils::SingleGuidArray(ShapeDefDataType::GUID),ShapeDefDataType::FriendlyName);

			//Position and velocity
			this->FHost->CreateValueInput("Position",2,ArrayUtils::Array2D(),TSliceMode::Dynamic,TPinVisibility::True,this->vInPosition);
			this->vInPosition->SetSubType2D(Double::MinValue,Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueInput("Velocity",2,ArrayUtils::Array2D(),TSliceMode::Dynamic,TPinVisibility::True,this->vInVelocity);
			this->vInVelocity->SetSubType2D(Double::MinValue,Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueInput("Angular Velocity",1,ArrayUtils::Array1D(),TSliceMode::Dynamic,TPinVisibility::True,this->vInAngularVelocity);
			this->vInAngularVelocity->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueInput("Is Bullet",1,ArrayUtils::Array1D(),TSliceMode::Dynamic,TPinVisibility::True,this->vInIsBullet);
			this->vInIsBullet->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,false,true,false);

			this->FHost->CreateValueInput("Do Create",1,ArrayUtils::Array1D(),TSliceMode::Dynamic,TPinVisibility::True,this->vInDoCreate);
			this->vInDoCreate->SetSubType(Double::MinValue,Double::MaxValue,0.01,0.0,true,false,false);

			//this->FHost->CreateValueOutput("Can Create",1,ArrayUtils::Array1D(),TSliceMode::Dynamic,TPinVisibility::True,this->vOutCanCreate);
			//this->vOutCanCreate->SetSubType(0,1,1,0,true,false,false);


			this->FHost->CreateNodeOutput("Body",TSliceMode::Dynamic,TPinVisibility::True,this->vOutBodies);
			this->vOutBodies->SetSubType(ArrayUtils::SingleGuidArray(BodyDataType::GUID),BodyDataType::FriendlyName);
			this->vOutBodies->SetInterface(this->mBodies);

			
		}

		void Box2dCreateBodyNode::Configurate(IPluginConfig^ Input)
		{

		}

		void Box2dCreateBodyNode::Evaluate(int SpreadMax)
		{
			double dblcreate;
			this->vInDoCreate->GetValue(0,dblcreate);

			this->mBodies->Reset();

			if (dblcreate >= 0.5 && this->vInWorld->IsConnected && this->vInShapes->IsConnected) 
			{
				if (this->mWorld->GetIsValid()) 
				{
					double x,y,vx,vy,va,bull;
					

					for (int i = 0; i < SpreadMax; i++) 
					{
						this->vInPosition->GetValue2D(i,x,y);
						this->vInVelocity->GetValue2D(i,vx,vy);
						this->vInAngularVelocity->GetValue(i,va);
						this->vInIsBullet->GetValue(i,bull);
						
						b2BodyDef bodydef;
						bodydef.position.Set(x,y);
						bodydef.isBullet = (bull >= 0.5);
						
						BodyCustomData* bdata = new BodyCustomData();
						
						bdata->Id = this->mWorld->GetNewBodyId();

						b2Body* body = this->mWorld->GetWorld()->CreateBody(&bodydef);
						body->SetLinearVelocity(b2Vec2(vx,vy));
						body->SetAngularVelocity(va);
						body->SetUserData(bdata);
						

						int realslice;
						this->vInShapes->GetUpsreamSlice(i % this->vInShapes->SliceCount,realslice);
						
						b2ShapeDef* shapedef = this->mShapes->GetSlice(realslice);
						
						b2Shape* shape = body->CreateShape(shapedef);
						
						ShapeCustomData* sdata = new ShapeCustomData();
						sdata->Id = this->mWorld->GetNewShapeId();
						shape->SetUserData(sdata);

						if (shapedef->density != 0.0) 
						{
							body->SetMassFromShapes();
						}

						//this->createdbodies->push_back(body);
						this->mBodies->Add(body);

					}
				}

				
			} 

			this->vOutBodies->SliceCount = this->mBodies->Size();
			this->vOutBodies->MarkPinAsChanged();

		}

		void Box2dCreateBodyNode::ConnectPin(IPluginIO^ Pin)
		{
			//cache a reference to the upstream interface when the NodeInput pin is being connected
        	if (Pin == this->vInWorld)
        	{
				INodeIOBase^ usI;
				this->vInWorld->GetUpstreamInterface(usI);
				this->mWorld = (WorldDataType^)usI;
        	}
			if (Pin == this->vInShapes) 
			{
				INodeIOBase^ usI;
				this->vInShapes->GetUpstreamInterface(usI);
				this->mShapes = (ShapeDefDataType^)usI;
			}
		}

		void Box2dCreateBodyNode::DisconnectPin(IPluginIO^ Pin)
		{
			if (Pin == this->vInWorld)
        	{
        		this->mWorld = nullptr;
        	}
			if (Pin == this->vInShapes)
        	{
        		this->mShapes = nullptr;
        	}
		}
	}
}