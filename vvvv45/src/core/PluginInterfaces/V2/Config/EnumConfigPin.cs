﻿using System;
using VVVV.PluginInterfaces.V1;

namespace VVVV.PluginInterfaces.V2.Config
{
	public class EnumConfigPin<T> : ConfigPin<T>
	{
		protected IEnumConfig FEnumConfigPin;
		protected Type FEnumType;
		
		public EnumConfigPin(IPluginHost host, ConfigAttribute attribute)
		{
			FEnumType = typeof(T);
			
			var entrys = Enum.GetNames(FEnumType);
			var defEntry = (attribute.DefaultEnumEntry != "") ? attribute.DefaultEnumEntry : entrys[0];
			host.UpdateEnum(FEnumType.Name, defEntry, entrys);
			
			host.CreateEnumConfig(attribute.Name, attribute.SliceMode, attribute.Visibility, out FEnumConfigPin);
			FEnumConfigPin.SetSubType(FEnumType.Name);
		}
		
		protected override IPluginConfig PluginConfig 
		{
			get 
			{
				return FEnumConfigPin;
			}
		}
		
		public override T this[int index]
		{
			get
			{
				string entry;
				FEnumConfigPin.GetString(index, out entry);

				return (T)Enum.Parse(FEnumType, entry);
			}
			set
			{
				FEnumConfigPin.SetString(index, value.ToString());
			}
		}
	}
}