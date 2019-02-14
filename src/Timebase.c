#include "Timebase.h"

/**
 * Convert video pts into frame index
 * @param  vid_ctx VideoContext
 * @param  pts     presentation timestamp of video frames
 * @return         >= 0 on success: frame index representation of pts param
 */
int64_t cov_video_pts(VideoContext *vid_ctx, int64_t pts) {
    if(pts < 0) {
        return -1;
    }
    AVStream *video_stream = get_video_stream(vid_ctx);
    if(!video_stream) {
        fprintf(stderr, "Video stream does not exist for VideoContext[%s]\n", vid_ctx->fmt_ctx->url);
        return -1;
    }
    int64_t timebase = video_stream->duration / video_stream->nb_frames;
    return pts / timebase;
}

/**
    Return >= 0 on success
*/
int64_t get_video_frame_pts(VideoContext *vid_ctx, int frameIndex) {
    if(frameIndex < 0) {
        return -1;
    }
    AVStream *video_stream = get_video_stream(vid_ctx);
    if(!video_stream) {
        fprintf(stderr, "Video stream does not exist\n");
        return -1;
    } else {
        // Duration of one frame in AVStream.time_base units
        int64_t timeBase = video_stream->duration / video_stream->nb_frames;
        return (int64_t)(frameIndex) * timeBase;
    }
}

int seek_video(VideoContext *vid_ctx, int frameIndex) {
    int64_t pts = get_video_frame_pts(vid_ctx, frameIndex);
    return seek_video_pts(vid_ctx, pts);
}

/**
    Return >= 0 on success
*/
int seek_video_pts(VideoContext *vid_ctx, int pts) {
    return av_seek_frame(vid_ctx->fmt_ctx, vid_ctx->video_stream_idx, pts, AVSEEK_FLAG_ANY);
}

int64_t get_audio_frame_pts(VideoContext *vid_ctx, int frameIndex) {
    int vid_pts = get_video_frame_pts(vid_ctx, frameIndex);
    if(vid_pts < 0) {
        return -1;
    }
    return cov_video_to_audio_pts(vid_ctx, vid_pts);
}

int64_t cov_video_to_audio_pts(VideoContext *vid_ctx, int videoFramePts) {
    return av_rescale_q(videoFramePts, get_video_stream(vid_ctx)->time_base,
                                       get_audio_stream(vid_ctx)->time_base);
}

// int seek_audio(VideoContext *vid_ctx, int frame) {
//     AVStream *audio_stream = get_audio_stream(vid_ctx);
//
// }