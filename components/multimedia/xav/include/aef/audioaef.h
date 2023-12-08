#ifndef _AUDIOAEF_H
#define _AUDIOAEF_H

#ifdef WIN32
#define DLL_API _declspec(dllexport)
#else
#define DLL_API
#endif

typedef int (*savefileCb)(char*filename,void*private_data);

#ifdef __cplusplus
extern "C" {
#endif
/*flag=1 则显示启动过程中的相关打印信息(printf输出)，默认关闭*/
DLL_API void audioaef_set_debug(int flag);
//fs,算法系统采样率
//conf_file, 音效配置文件；
// outbits，process输出数据每个样点bit数(有符号数)，支持16，24； 与输入数据的bit数无关
//maxFrameLen,允许的最大输入帧长
DLL_API void* audioaef_create(int infs, int outfs, char*conf_file, int outBits, int maxFrameLen);
//同上,配置从字符串buffer中读取不从文件读取
DLL_API void* audioaef_create_fromBuffer(int infs, int outfs, char*buffer, int bufsize,int outBits, int maxInFrameLen);
//DLL_API int audioaef_refreshConfig_fromBuffer(void*ins,char*buffer, int bufsize);
//修改采样率
DLL_API int audioaef_set_fs(void*ins,int infs, int outfs);
//设置接收网络保存配置的回调函数，如无则不相应网络保存命令
DLL_API int audioaef_set_saveFileCb(void*ins,savefileCb func,void*private_data);

//相应网络保存配置指令，file被忽略
DLL_API int aduioaef_net_saveconfig(void*ins,char*file);
//相应网络保存配置指令，file被忽略，size为buffer内存大小,成功返回写入buffer大小（含字符串结束符）
DLL_API int audioaef_save_config2Buffer(void*ins,char*buffer, int size);
DLL_API int audioaef_save_config(void*ins,char*file);

//根据输入帧长计算最大输出帧长；
//如果输入输出采样率不同，则process每次返回结果会不固定长度，此接口返回最大长度，便于buffer和播放管理
DLL_API int audioaef_getMaxOutFrame(void*ins,int inFrameLen);
//对输入语音做流程中的算法处理，数据格式（左右通道交错形式）
//outdata输出数据，长度可根据输入参数计算所得，也可以从函数返回值获取；
//channels : 输入数据通道数 1 or 2
//bitspersample: 16 or 24,24bit格式每个sample占据32bit内存空间
//outChans：输出通道数
//return ： outdata 数据字节长度
DLL_API int audioaef_process(void*ins,char* indata, int lenOfBytes, int channels,int bitspersample, char*outdata,int outChans);


DLL_API void audioaef_destroy(void*ins);
//net-server init for audio config api
//aefIns 网络配置对象为aefIns的音效实例
DLL_API void* audioRPC_init(void*aefIns); //return RPC handle
DLL_API void audioRPC_close(void* handle);

#ifdef __cplusplus
}
#endif

#endif
