/*********************************************************************************
  *Copyright(C),2018, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_ipc_stream_storage.h
**********************************************************************************/

#ifndef __TUYA_IPC_STREAM_STORAGE_H__
#define __TUYA_IPC_STREAM_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_ipc_media.h"

#define SS_BASE_PATH_LEN            128

/**
 * \brief SD卡储存管理结构体
 * \struct TUYA_IPC_TRANSFER_VAR_S
 */
typedef struct
{
    CHAR base_path[SS_BASE_PATH_LEN]; /**< SD卡基准存储路径 */
    TUYA_CODEC_ID video_codec;       /**< 视频编码类型 */
    TUYA_CODEC_ID audio_codec;       /**< 音频编码类型 */

    UINT max_key_num; /**< 单个文件中的最大h264关键帧个数 */

    VOID *p_reserved;
}STREAM_STORAGE_INFO_S;

/**
 * \brief SD卡文件录入的方式
 * \enum STREAM_STORAGE_WRITE_MODE_E
 */
typedef enum {
    SS_WRITE_MODE_ALL,  /**< 全时写入SD卡 */
    SS_WRITE_MODE_EVENT,/**< 事件发生时写入SD卡 */
    SS_WRITE_MODE_NONE, /**< 不写入SD卡 */
}STREAM_STORAGE_WRITE_MODE_E;

/**
 * \fn OPERATE_RET tuya_ipc_ss_init(IN STREAM_STORAGE_INFO_S *p_ss_info)
 * \brief 初始化SD卡存储逻辑
 * \param[in] p_ss_info 信息指针
 * \param[in] write_mode 存储模式
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_init(IN STREAM_STORAGE_INFO_S *p_ss_info, STREAM_STORAGE_WRITE_MODE_E write_mode);

/**
 * \fn OPERATE_RET tuya_ipc_ss_set_write_mode(IN CONST INT write_mode)
 * \brief 设置存储模式
 * \param[in] p_h264_buf h264码流
 * \param[in] buf_len h264码流长度
 * \param[in] key_frame 是否是关键帧
 * \return OPERATE_RET
 */
VOID tuya_ipc_ss_set_write_mode(IN CONST STREAM_STORAGE_WRITE_MODE_E write_mode);

/**
 * \fn VOID tuya_ipc_ss_trigger_event(VOID);
 * \brief 触发移动侦测录像
 * \return OPERATE_RET
 */
VOID tuya_ipc_ss_trigger_event(VOID);

/**
 * \fn VOID tuya_ipc_ss_trigger_event(VOID);
 * \brief 清空文件
 * \return OPERATE_RET
 */
VOID tuya_ipc_ss_delete_all_files(VOID);


/**
 * \fn OPERATE_RET tuya_ipc_ss_put_h264(IN CONST MEDIA_FRAME_S *p_video_frame)
 * \brief 添加视频码流
 * \param[in] p_h264_buf h264码流
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_put_video(IN CONST MEDIA_FRAME_S *p_video_frame);

/**
 * \fn OPERATE_RET tuya_ipc_ss_put_pcm(IN CONST tuya_ss_pcm_frame *p_audio_frame)
 * \brief 添加音频码流
 * \param[in] p_pcm_buf pcm码流
 * \param[in] buf_len pcm码流长度
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_put_audio(IN CONST MEDIA_FRAME_S *p_audio_frame);

/**
 * \fn OPERATE_RET tuya_ipc_ss_query_by_month(IN USHORT query_year, IN USHORT query_month, OUT UINT *p_return_days)
 * \brief 查询指定年-月下的SD卡本地存储数据的天数
 * \param[in] query_year 要查询的年
 * \param[in] query_month 要查询的月
 * \param[out] p_return_days 有效日期
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_pb_query_by_month(IN USHORT query_year, IN USHORT query_month, OUT UINT *p_return_days);

/**
 * \brief 单个文件的时间戳信息
 * \struct SS_FILE_TIME_TS_S
 */
typedef struct
{
    UINT start_timestamp; /**< 回放文件开始时间戳（以秒为单位） */
    UINT end_timestamp;   /**< 回放文件结束时间戳（以秒为单位） */
} SS_FILE_TIME_TS_S;

/**
 * \struct SS_QUERY_DAY_TS_ARR_S
 */
typedef struct
{
    UINT file_count; /**< 当天有多少个回放文件 */
    SS_FILE_TIME_TS_S file_arr[0]; /**< 回放文件数组 */
} SS_QUERY_DAY_TS_ARR_S;

/**
 * \fn OPERATE_RET tuya_ipc_ss_query_by_day( IN USHORT query_year, IN USHORT query_month, IN UCHAR query_day,
                                      OUT SS_QUERY_DAY_TS_ARR_S **p_p_query_ts_arr)
 * \brief 查询指定年-月-日下的SD卡本地存储数据的详细信息
 * \param[in] query_year 要查询的年
 * \param[in] query_month 要查询的月
 * \param[in] query_day 要查询的日
 * \param[out] p_p_query_ts_arr 有效数据详细信息
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_pb_query_by_day( IN USHORT year, IN USHORT month, IN UCHAR day,
                                      OUT SS_QUERY_DAY_TS_ARR_S **pTsArr);

/**
 * \fn OPERATE_RET tuya_ipc_ss_query_free_ts_arr( IN SS_QUERY_DAY_TS_ARR_S *p_query_ts_arr)
 * \brief 释放SDK返回的内存
 * \param[in] p_query_ts_arr 要释放的内存
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_pb_query_free_ts_arr( IN SS_QUERY_DAY_TS_ARR_S *p_query_ts_arr);

/**
 * \brief 回放事件
 * \struct SS_PB_EVENT_E
 */
typedef enum
{
    SS_PB_FINISH = 0, /**< 当前文件播放完毕 */
    SS_PB_NEWFILE,    /**< 找到连续文件，继续播放 */
}SS_PB_EVENT_E;

/**
 * \typedef SS_PB_EVENT_CB
 * \brief 回放事件回调函数定义
 * \param[in] pb_idx 回放事务ID
 * \param[in] pb_event 回放事件ID
 * \param[in] args 额外参数
 */
typedef VOID (*SS_PB_EVENT_CB)(IN UINT pb_idx, IN SS_PB_EVENT_E pb_event, IN PVOID args);

/**
 * \typedef SS_PB_GET_MEDIA_CB
 * \brief 回放数据获取定义
 * \param[in] pb_idx 回放事务ID
 * \param[in] p_frame 回放数据
 */
typedef VOID (*SS_PB_GET_MEDIA_CB)(IN UINT pb_idx, IN CONST MEDIA_FRAME_S *p_frame);

/**
 * \fn OPERATE_RET tuya_ipc_ss_pb_start(OUT UINT *p_pb_idx, IN SS_PB_EVENT_CB event_cb, IN SS_PB_GET_MEDIA_CB video_cb, IN SS_PB_GET_MEDIA_CB audio_cb)
 * \brief 开始一个新的回放事务
 * \param[OUT] p_pb_idx 事务ID
 * \param[in] event_cb 回放事件回调函数
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_pb_start(IN UINT pb_idx, IN SS_PB_EVENT_CB event_cb, IN SS_PB_GET_MEDIA_CB video_cb, IN SS_PB_GET_MEDIA_CB audio_cb);

/**
 * \brief 回放状态
 * \struct SS_PB_STATUS_E
 */
typedef enum
{
    SS_PB_MUTE, /**< 静音 */
    SS_PB_UN_MUTE, /**< 取消静音 */
    SS_PB_PAUSE, /**< 暂停 */
    SS_PB_RESUME,/**< 继续 */
}SS_PB_STATUS_E;

/**
 * \fn OPERATE_RET tuya_ipc_ss_pb_set_status(IN UINT pb_idx, IN SS_PB_STATUS_E new_status)
 * \brief 设置回放事务的属性
 * \param[in] pb_idx 事务ID
 * \param[in] new_status 状态
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_pb_set_status(IN UINT pb_idx, IN SS_PB_STATUS_E new_status);

/**
 * \fn OPERATE_RET tuya_ipc_ss_pb_stop(IN UINT pb_idx)
 * \brief 停止一个回放事务
 * \param[in] pb_idx 事务ID
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_pb_stop(IN UINT pb_idx);

/**
 * \fn OPERATE_RET tuya_ipc_ss_pb_seek(IN UINT pb_idx, IN SS_FILE_TIME_TS_S *pb_file_info, IN UINT play_timestamp)
 * \brief 对特定的回放事务进行Seek
 * \param[in] pb_idx 事务ID
 * \param[in] pb_file_info seek的文件
 * \param[in] play_timestamp seek的绝对时间
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_pb_seek(IN UINT pb_idx, IN SS_FILE_TIME_TS_S *pb_file_info, IN UINT play_timestamp);

/**
 * \fn OPERATE_RET tuya_ipc_ss_pb_stop_all(VOID)
 * \brief 停止全部回放事务
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ss_pb_stop_all(VOID);



#ifdef __cplusplus
}
#endif

#endif
