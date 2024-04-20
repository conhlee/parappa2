#include "main/scrctrl.h"

/* data */
extern u_int thnum_tbl[4];
extern SCR_SND_DBUFF scr_snd_dbuff;

/* sdata */
/* static */ int titleStartKey;
/* static */ int fadeoutStartKey;
/* static */ int gameEndWaitLoop;
/* static */ int replayGuiOffFlag;
/* static */ int jimakuWakuOff;
int currentTblNumber;
int vs_tapdat_tmp_cnt;
int scrJimakuLine;
int scrDrawLine;
int scrMbarLine;
int scrRefLineTime;

/* sdata - static */
/* implement on ScrCtrlMainLoop */
int dbgmsg_on_off;

/* bss */
extern SCORE_INDV_STR score_indv_str[5];

/* sbss - static */
int follow_scr_tap_memory_cnt;
int follow_scr_tap_memory_cnt_load;
int commake_str_cnt;

/* .lit4 */
float FLT_00398F10;

int GetCurrentTblNumber(void)
{
    return currentTblNumber;
}

DISP_LEVEL RANK_LEVEL2DISP_LEVEL(RANK_LEVEL lvl)
{
    DISP_LEVEL lvl_tbl[17] =
    {
        DLVL_COOL,  DLVL_COOL,
        DLVL_GOOD,  DLVL_GOOD,  DLVL_GOOD,
        DLVL_BAD,   DLVL_BAD,   DLVL_BAD,
        DLVL_AWFUL, DLVL_AWFUL, DLVL_AWFUL,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX
    };

    return lvl_tbl[lvl];
}

DISP_LEVEL RANK_LEVEL2DISP_LEVEL_HK(RANK_LEVEL lvl)
{
    DISP_LEVEL lvl_tbl[15] =
    {
        DLVL_COOL,  DLVL_COOL,
        DLVL_GOOD,  DLVL_GOOD,  DLVL_GOOD,
        DLVL_BAD,   DLVL_BAD,   DLVL_BAD,
        DLVL_AWFUL, DLVL_AWFUL, DLVL_AWFUL,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX, 5
    };

    return lvl_tbl[lvl];
}

void ScrTapDbuffCtrlInit(void *data_top, int bk0, int bk1)
{
    scr_snd_dbuff.bank[0]      = bk0;
    scr_snd_dbuff.bank[1]      = bk1;

    scr_snd_dbuff.data_top     = data_top;
    scr_snd_dbuff.next_index   = 0;

    scr_snd_dbuff.sndrec_pp[0] = NULL;
    scr_snd_dbuff.sndrec_pp[1] = NULL;
}

INCLUDE_ASM(const s32, "main/scrctrl", ScrTapDbuffSet);

void ScrTapDbuffSetSp(SNDREC *sndrec_pp, int id)
{
    u_int ret;

    if (id > -1)
    {
        scr_snd_dbuff.next_index = id;
        ScrTapDataTrans(sndrec_pp, scr_snd_dbuff.bank[id & 1 ^ 1], scr_snd_dbuff.data_top);
        scr_snd_dbuff.sndrec_pp[id & 1 ^ 1] = sndrec_pp;
    }
}

void ScrTapDbuffClear(void)
{
    scr_snd_dbuff.next_index = 0;
    scr_snd_dbuff.sndrec_pp[0] = NULL;
    scr_snd_dbuff.sndrec_pp[1] = NULL;
}

void ScrTapCtrlInit(void *data_top)
{
    ScrTapDbuffCtrlInit(data_top, 1, 2);
}

INCLUDE_ASM(const s32, "main/scrctrl", ScrTapDataTrans);

INCLUDE_ASM(const s32, "main/scrctrl", ScrTapDataTransCheck);

INCLUDE_ASM(const s32, "main/scrctrl", ScrTapReq);

INCLUDE_ASM(const s32, "main/scrctrl", ScrTapReqStop);

INCLUDE_ASM(const s32, "main/scrctrl", exam_tbl_updownInit);
void exam_tbl_updownInit(SCORE_INDV_STR *sindv_pp);

INCLUDE_ASM(const s32, "main/scrctrl", exam_tbl_updownSet);

INCLUDE_ASM(const s32, "main/scrctrl", exam_tbl_updownChange);

INCLUDE_ASM(const s32, "main/scrctrl", vsTapdatSetMemorySave);

INCLUDE_ASM(const s32, "main/scrctrl", vsTapdatSetMemoryLoad);

INCLUDE_ASM(const s32, "main/scrctrl", vsTapdatSet);

INCLUDE_ASM(const s32, "main/scrctrl", vsTapdatSetMoto);

INCLUDE_ASM(const s32, "main/scrctrl", followTapInit);

INCLUDE_ASM(const s32, "main/scrctrl", followTapSave);

INCLUDE_ASM(const s32, "main/scrctrl", followTapLoad);

INCLUDE_ASM(const s32, "main/scrctrl", ScrLincChangTbl);

INCLUDE_ASM(const s32, "main/scrctrl", ScrLincChangTblRef);

INCLUDE_ASM(const s32, "main/scrctrl", ScrLineSafeRefMode);

INCLUDE_ASM(const s32, "main/scrctrl", ScrDrawTimeGet);

INCLUDE_ASM(const s32, "main/scrctrl", ScrDrawTimeGetFrame);

INCLUDE_ASM(const s32, "main/scrctrl", KeyCntClear);
void KeyCntClear(int *key_pp);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlCurrentSearch);
SCRREC* ScrCtrlCurrentSearch(SCORE_INDV_STR *sindv_pp, int index, int frame);

void ScrCtrlIndvInit(STDAT_DAT *sdat_pp)
{
	int             i, j;
	int             dare;

	SCORE_INDV_STR *sindv_pp;
    GLOBAL_PLY     *gply_pp;

    WorkClear(score_indv_str, sizeof(score_indv_str));
    sindv_pp = score_indv_str;

    for (i = 0; i < 5; i++, sindv_pp++)
    {
        dare = -1;
        gply_pp = global_data.global_ply;
        
        for (j = 0; j < 4; j++)
        {
            if (gply_pp[j].player_code == PR_BIT(i))
            {
                dare = j;
                break;
            }
        }

        if (dare >= 0)
        {
            KeyCntClear(sindv_pp->keyCnt);
            tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));

            sindv_pp->keyCntCom = 0;
            sindv_pp->status = 1;
            sindv_pp->plycode = PR_BIT(i);
            sindv_pp->global_ply = &gply_pp[dare];
            sindv_pp->top_scr_ctrlpp = sdat_pp->scr_pp->scr_ctrl_pp;
            
            sindv_pp->current_scrrec_pp = ScrCtrlCurrentSearch(sindv_pp, global_data.draw_tbl_top, 0);
            sindv_pp->useLine = global_data.draw_tbl_top;
            
            sindv_pp->global_ply->exam_tbl_up = 0;
            sindv_pp->global_ply->exam_tbl_dw = 0;

            exam_tbl_updownInit(sindv_pp);

            for (j = 0; j < 24; j++)
            {
                sindv_pp->sjob[j] = -1;
            }

            sindv_pp->tapset_pos = -1;
        }
    }
}

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlExamClear);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlExamClearIndv);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlIndvNextTime);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlIndvNextReadLine);

INCLUDE_ASM(const s32, "main/scrctrl", getLvlTblRand);

INCLUDE_ASM(const s32, "main/scrctrl", tapLevelChangeSub);

INCLUDE_ASM(const s32, "main/scrctrl", tapLevelChange);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlIndvNextRead);

INCLUDE_ASM(const s32, "main/scrctrl", intIndvStatusSet);

INCLUDE_ASM(const s32, "main/scrctrl", allIndvNextContinue);

INCLUDE_ASM(const s32, "main/scrctrl", allIndvGoContinue);

INCLUDE_ASM(const s32, "main/scrctrl", otherIndvPause);

INCLUDE_ASM(const s32, "main/scrctrl", otherIndvTapReset);

INCLUDE_ASM(const s32, "main/scrctrl", selectIndvTapResetPlay);

INCLUDE_ASM(const s32, "main/scrctrl", IndivMoveChange);

INCLUDE_ASM(const s32, "main/scrctrl", useIndevAllMove);

INCLUDE_ASM(const s32, "main/scrctrl", useIndevCodeGet);

INCLUDE_ASM(const s32, "main/scrctrl", targetTimeGet);

INCLUDE_ASM(const s32, "main/scrctrl", useIndevSndKill);

INCLUDE_ASM(const s32, "main/scrctrl", useAllClearKeySnd);

INCLUDE_ASM(const s32, "main/scrctrl", useIndevSndKillOther);

INCLUDE_ASM(const s32, "main/scrctrl", TapKeyCheckNum);

INCLUDE_ASM(const s32, "main/scrctrl", TapKeyGetDatPP);

INCLUDE_ASM(const s32, "main/scrctrl", tapReqGroupInit);

INCLUDE_ASM(const s32, "main/scrctrl", tapReqGroupTapClear);

INCLUDE_ASM(const s32, "main/scrctrl", tapReqGroup);

INCLUDE_ASM(const s32, "main/scrctrl", tapReqGroupPoll);

INCLUDE_ASM(const s32, "main/scrctrl", tapEventCheck);

INCLUDE_ASM(const s32, "main/scrctrl", otehon_all_make);

INCLUDE_ASM(const s32, "main/scrctrl", treateTimeChange);

//INCLUDE_ASM(const s32, "main/scrctrl", thnum_get);
int thnum_get(int p96_num, CK_TH_ENUM ckth)
{
    u_int thnum_data;
    int   ck_bit;
    int   ck_dat;
    int   ret_cnt;
    int   i;

    ck_bit = -1;
    ret_cnt = 0;
    thnum_data = thnum_tbl[ckth];

    for (i = 0; i <= p96_num; i++)
    {
        ck_dat = (thnum_data >> (23 - i % 24)) & 1;
        
        if (ck_bit < 0)
        {
            if (ck_dat == 0)
                ret_cnt = 1;
        }
        else if (ck_dat != ck_bit)
        {
            ret_cnt++;
        }
        
        ck_bit = ck_dat;
    }

    return ret_cnt;
}

INCLUDE_ASM(const s32, "main/scrctrl", MapNormalNumGet);

INCLUDE_ASM(const s32, "main/scrctrl", on_th_make);

INCLUDE_ASM(const s32, "main/scrctrl", exh_normal_add);

INCLUDE_ASM(const s32, "main/scrctrl", exh_normal_sub);

INCLUDE_ASM(const s32, "main/scrctrl", exh_nombar_sub);

INCLUDE_ASM(const s32, "main/scrctrl", exh_mbar_key_out);

INCLUDE_ASM(const s32, "main/scrctrl", exh_mbar_time_out);

INCLUDE_ASM(const s32, "main/scrctrl", exh_mbar_num_out);

INCLUDE_ASM(const s32, "main/scrctrl", exh_yaku);

INCLUDE_ASM(const s32, "main/scrctrl", exh_yaku_original);

INCLUDE_ASM(const s32, "main/scrctrl", exh_yaku_hane);

INCLUDE_ASM(const s32, "main/scrctrl", exh_allkey_out);

INCLUDE_ASM(const s32, "main/scrctrl", exh_allkey_out_nh);

INCLUDE_ASM(const s32, "main/scrctrl", exh_command);

INCLUDE_ASM(const s32, "main/scrctrl", exh_renda_out);

INCLUDE_ASM(const s32, "main/scrctrl", manemane_check_sub);

INCLUDE_ASM(const s32, "main/scrctrl", manemane_check);

INCLUDE_ASM(const s32, "main/scrctrl", exh_mane);

INCLUDE_ASM(const s32, "main/scrctrl", exh_all_add);

INCLUDE_ASM(const s32, "main/scrctrl", IndvGetTapSetAdrs);

INCLUDE_ASM(const s32, "main/scrctrl", nextExamTime);

INCLUDE_ASM(const s32, "main/scrctrl", GetSindvPcodeLine);

INCLUDE_ASM(const s32, "main/scrctrl", ExamScoreCheck);

INCLUDE_ASM(const s32, "main/scrctrl", ExamScoreCheckSame);

INCLUDE_ASM(const s32, "main/scrctrl", levelChangeCheck);

INCLUDE_ASM(const s32, "main/scrctrl", levelUpRank);

INCLUDE_ASM(const s32, "main/scrctrl", levelDownRank);

INCLUDE_RODATA(const s32, "main/scrctrl", D_00392D30);

INCLUDE_RODATA(const s32, "main/scrctrl", D_00392D40);

INCLUDE_RODATA(const s32, "main/scrctrl", D_00392D88);

INCLUDE_ASM(const s32, "main/scrctrl", ScrMoveSetSub);

INCLUDE_ASM(const s32, "main/scrctrl", ScrExamSetCheck);

INCLUDE_ASM(const s32, "main/scrctrl", subjobEvent);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlIndvJob);

INCLUDE_ASM(const s32, "main/scrctrl", ScrTimeRenew);

INCLUDE_ASM(const s32, "main/scrctrl", ScrMbarReq);

INCLUDE_ASM(const s32, "main/scrctrl", allTimeCallbackTimeSetChanTempo);

INCLUDE_ASM(const s32, "main/scrctrl", SetIndvDrawTblLine);

INCLUDE_ASM(const s32, "main/scrctrl", otehonSetCheck);

// INCLUDE_RODATA(const s32, "main/scrctrl", D_00392F70);
INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlMainLoop);

INCLUDE_ASM(const s32, "main/scrctrl", GetTimeType);

INCLUDE_ASM(const s32, "main/scrctrl", GetTimeOfset);

INCLUDE_ASM(const s32, "main/scrctrl", GetSubtLine);

INCLUDE_ASM(const s32, "main/scrctrl", GetDrawLine);

INCLUDE_ASM(const s32, "main/scrctrl", GetLineTempo);

INCLUDE_ASM(const s32, "main/scrctrl", SetLineChannel);

INCLUDE_ASM(const s32, "main/scrctrl", SetIndvCdChannel);

INCLUDE_ASM(const s32, "main/scrctrl", CheckIndvCdChannel);

// INCLUDE_RODATA(const s32, "main/scrctrl", dbg_tbl_msg);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlInit);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlQuit);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlInitCheck);

INCLUDE_ASM(const s32, "main/scrctrl", ScrCtrlGoLoop);

INCLUDE_ASM(const s32, "main/scrctrl", ScrEndCheckScore);

INCLUDE_ASM(const s32, "main/scrctrl", ScrEndCheckTitle);

INCLUDE_ASM(const s32, "main/scrctrl", ScrEndCheckFadeOut);

INCLUDE_ASM(const s32, "main/scrctrl", ScrEndWaitLoop);

INCLUDE_ASM(const s32, "main/scrctrl", bonusGameInit);

INCLUDE_ASM(const s32, "main/scrctrl", bonusGameCntPls);

INCLUDE_ASM(const s32, "main/scrctrl", bonusPointSave);

INCLUDE_ASM(const s32, "main/scrctrl", bngTapEventCheck);

INCLUDE_ASM(const s32, "main/scrctrl", bonusGameParaReq);

INCLUDE_ASM(const s32, "main/scrctrl", bonusGameKoamaReq);

INCLUDE_ASM(const s32, "main/scrctrl", bonus_minus_point_sub);

INCLUDE_ASM(const s32, "main/scrctrl", bonus_pls_point_sub);

INCLUDE_ASM(const s32, "main/scrctrl", bonusGameCtrl);

static u_long hex2dec(u_long data)
{
    u_long ret = 0;
    u_int i;

    for (i = 0; data != 0;)
    {
        ret |= (data % 10) << (i * 4);
        data /= 10;

        i++;
        
        if (i >= 16)
            return ret;
    }

    return ret;
}

INCLUDE_ASM(const s32, "main/scrctrl", bnNumberDisp);

INCLUDE_ASM(const s32, "main/scrctrl", bonusScoreDraw);

INCLUDE_ASM(const s32, "main/scrctrl", set_lero_gifset);

INCLUDE_ASM(const s32, "main/scrctrl", LessonRoundDisp);
