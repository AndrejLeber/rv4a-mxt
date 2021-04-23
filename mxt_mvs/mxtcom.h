#ifndef MXTCOM_H
#define MXTCOM_H

#include "strdef.h"
#include <string.h>

// Prepare MXT command for recieving POSE of the robot (NO MOVEMENT!)
void mxt_prep_recv_pos(MXTCMD &cmd) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_NULL;
    cmd.SendType = MXT_TYP_NULL;
    cmd.RecvType = MXT_TYP_POSE;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_POSE;
    cmd.RecvType2 = MXT_TYP_JOINT;
    cmd.RecvType3 = MXT_TYP_PULSE;
}

// Preparing MXT command for moving the robot to POSE (MOVEMENT!)
void mxt_prep_move_pos(MXTCMD &cmd) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_MOVE;
    cmd.SendType = MXT_TYP_POSE;
    cmd.RecvType = MXT_TYP_POSE;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_POSE;
    cmd.RecvType2 = MXT_TYP_JOINT;
    cmd.RecvType3 = MXT_TYP_PULSE;
}

// End MXT command
void mxt_prep_end(MXTCMD &cmd) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_END;
    cmd.SendType = MXT_TYP_NULL;
    cmd.RecvType = MXT_TYP_NULL;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_NULL;
    cmd.RecvType2 = MXT_TYP_NULL;
    cmd.RecvType3 = MXT_TYP_NULL;
}



#endif // MXTCOM_H
