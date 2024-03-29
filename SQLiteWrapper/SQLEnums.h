#ifndef SQLEnums_hpp
#define SQLEnums_hpp


#define RET_VAL_GROUP(retType) typename std::enable_if<std::retType<T>::value, T>::type
#define RET_VAL_SAME(retType) typename std::enable_if<std::is_same<T, retType>::value, T>::type


struct SQLEnums 
{
	enum OpenMode 
	{
		Read = 0x00000001,
		ReadWrite = 0x00000002,
		Create = 0x00000004,
		Uri = 0x00000040,
		Memory = 0x00000080,
		NoMutex = 0x00008000,
		FullMutex = 0x00010000,
		SharedCache = 0x00020000,
		PrivateCache = 0x00040000
	};

	enum ValueDataType
	{
		Integer = 1,
		Float = 2,
		String = 3,
		Blob = 4,
		Null = 5
	};
};

#endif