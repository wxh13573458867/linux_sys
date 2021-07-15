#ifndef SAFE_SHM_H
#define SAFE_SHM_H

#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/*
 *@brief	获取信号量集(只使用信号量集中的第一个信号量)
 *@param:file	文件名
 *@param:proj	子序列号
 *@param:semid	传出参数:信号量集ID
 *@return	成功:0	失败:-1
 **/
extern int SEM_Init(const char *file, int proj, int *semid);

/*
 *@brief	销毁坐标文件及信号量集
 *@param:file	文件名
 *@param:proj	子序列号
 *@return	无
 **/
extern void SEM_Destroy(const char *file, int proj);

/*
 *@brief	信号量加锁
 *@param:semid	信号量集ID
 *@return	成功:0	失败:-1
 **/
extern int SEM_locked(int semid);

/*
 *@brief	信号量解锁
 *@param:semid	信号量集ID
 *@return	成功:0	失败:-1
 **/
extern int SEM_unlock(int semid);

/*
 *@brief	加锁访问资源,结束后解锁
 *@param:semid	信号量集ID
 *@param:handle	函数指针(用户自定义函数,加锁调用)
 *@param:arg	用户自定义参数
 *@return	成功:0	失败:-1
 **/
extern int SEM_Visit(int semid, void(* handle)(void *), void *arg);

/*
 *@brief	获取共享内存首地址
 *@param:file	文件名
 *@param:proj	子序列号
 *@param:shmsize共享内存大小(不是4k的整倍数,自动补齐)
 *@param:shmbuf	传出参数:指向共享内存首地址的指针
 *@return	成功:0	失败:-1
 **/
extern int SHM_Init(const char *file, int proj, int *shmsize, void **shmbuf);

/*
 *@brief	取消关联共享内存
 *@param:shmbuf	指向共享内存首地址的指针
 *@return	无
 **/
extern void SHM_Secede(void **shmbuf);

/*
 *@brief	销毁坐标文件及共享内存
 *@param:file	文件名
 *@param:proj	子序列号
 *@return	无
 **/
extern void SHM_Destroy(const char *file, int proj);

/*
 *@brief	读共享内存
 *@param:shmbuf	共享内存首地址
 *@param:index	读取共享内存的位置
 *@param:outbuf 读缓冲区
 *@param:outbufSize	读取数据大小
 *@return	成功:0	失败:-1
 **/
extern int SHM_Read(void *shmbuf, int index, void *outbuf, int outbufSize);

/*
 *@brief	写共享内存
 *@param:shmbuf	共享内存首地址
 *@param:index	写入共享内存的位置
 *@param:inbuf 写缓冲区
 *@param:inbufSize	写入数据大小
 *@return	成功:0	失败:-1
 **/
extern int SHM_Write(void *shmbuf, int index, void *inbuf, int inbufSize);

#endif
