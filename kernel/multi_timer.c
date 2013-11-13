#include "kernel.h"
#include "memory.h"
#include "c_lib/lib.h"
#include "target/driver/timer_driver.h"
#include "multi_timer.h"


/**************************************************************************
 * 以下は割込みハンドラであり，非同期で呼ばれるので，ライブラリ関数などを 			*
 * 呼び出す場合には注意が必要．																							*
 * 基本として，以下のいずれかに当てはまる関数しか呼び出してはいけない．				*
 * ・再入可能である(そんなのはない)．																				*
 * ・スレッドから呼ばれることは無い関数である．																*
 * ・スレッドから呼ばれることがあるが，割込み禁止で呼び出している．						*
 * また非コンテキスト状態で呼ばれるため，システム・コールは利用してはいけない．	*
 * (サービス・コールを利用すること)																					*
 **************************************************************************/


/*! 差分のキューへタイマコントロールブロック挿入 */
static void insert_tmrcb_diffque(TMRCB* newtbf);

/*! 差分のキューからノードを進める */
static void next_tmrcb_diffque(void);


/*! 周期タイマハンドラ */
void cyclic_timer_handler1(void)
{
	/* ハードタイマ */
	//if (is_check_timer_expired(HARD_TIMER_DEFAULT_DEVICE)) { /* タイマは満了したか */
		DEBUG_OUTMSG("timer_handler1!\n");
		expire_cycle_timer(0); /* タイマ満了処理 */
		//}
}


/*! ワンショットタイマハンドラ */
void oneshot_timer_handler1(void)
{
	//CYCCB *cycb = (CYCCB *)mg_timerque.tmrhead->rqobjp; /* 周期タイマ以外の場合は0となる */
	//TMRCB *tbf;
	
	/* 差分のキューによるソフトタイマ */
	DEBUG_OUTMSG("softtimer is expire 1.\n\n");
	expire_oneshot_timer(1);
  cancel_timer(mg_timerque.index); /* タイマキャンセル処理 */
	next_tmrcb_diffque(); /* 差分のキューからタイマコントロールブロックの排除 */
	
	/* 周期タイマの時はタイマブロックを作成(createを呼ぶと先頭のタイマがかけられてしまうので排除してから呼ぶ) */
  //if (cycb != NULL) {
		//tbf = (TMRCB *)create_tmrcb_diffque(OTHER_MAKE_TIMER, cycb->cyctim, (TMRRQ_OBJP)cycb, cycb->func, cycb->exinf); /* 差分のキューのノードを作成 */
		//cycb->tobjp = (TMR_OBJP)tbf;
  //}
}


/*!
 * タイマドライバの初期化
 */
void tmrdriver_init(void)
{
	mg_timerque.tmrhead = NULL;
	mg_timerque.index = 1;
}


/*!
 * 差分のキューのノードを作成
 * 多少のすれ違い(タイマ割込みによるノード解放とシステムコールによるノード作成)の考慮
 * flag : タイマの要求種類
 * request_sec : 要求タイマ値
 * rqobjp : ソフトタイマを要求したオブジェクトのポインタ(ソフトタイマで周期機能を使用したいケースのみ指定する.)
 * func : タイマ満了時のコールバックルーチン(funcなしで呼んではいけない)
 * *argv : コールバックルーチンに渡すパラメータ
 * (返却値)newtbf : 新規作成したタイマコントロールブロックへポインタ
*/
OBJP create_tmrcb_diffque(short flag, int request_sec, TMRRQ_OBJP rqobjp, TMR_CALLRTE func, void *argv)
{
	TMRCB *newtbf;

	newtbf = (TMRCB *)get_mpf_isr(sizeof(*newtbf)); /* 動的メモリ取得要求 */

	/* メモリが取得できない */
  if(newtbf == NULL) {
    down_system();
  }
  
	newtbf->next = newtbf->prev = NULL;
	newtbf->flag = flag;
	newtbf->usec = request_sec;
	newtbf->rqobjp = rqobjp; /* 周期機能を使用しない時は0が入る */
	newtbf->func = func;
	newtbf->argv = argv;
	
	insert_tmrcb_diffque(newtbf); /* 差分のキューへタイマコントロールブロックの挿入 */
	
	return (OBJP)newtbf;
}


/*!
 * 差分のキューへタイマコントロールブロック挿入
 * *newtbf : 挿入するタイマコントロールブロック
 * (返却値)newtbf : 挿入したタイマコントロールブロックへポインタ
 */
static void insert_tmrcb_diffque(TMRCB* newtbf)
{
	TMRCB *worktbf, *tmptbf;
	int time_now = 0; /* 現在までカウントしたタイマ値 */
	int diff_usec; /* 差分タイマ値 */
	int i;

	/* 現在までカウントタイマ値を取得 */
  if (mg_timerque.tmrhead != NULL) {
    time_now = (int)get_timervalue(mg_timerque.index);
  }

	/* ここから差分のキューに挿入 */
	tmptbf = worktbf = mg_timerque.tmrhead;
	/* 差分のキューにノードがない場合 */
	if (worktbf == NULL) {
		mg_timerque.tmrhead = newtbf;
		start_oneshot_timer(mg_timerque.index, mg_timerque.tmrhead->usec); /*新規作成したnewtbfのタイマをスタートさせる*/
		return;
	}
	/* 差分のキューにノードがある場合 */
	else {
		/* (forの継続条件でtmptbf != NULLは指定できない) */
		for (i = 0; worktbf != NULL; i++) {
			diff_usec = worktbf->usec - time_now; /* 現在ノードのタイマ値 - 現在までカウントしたタイマ値 */
			/* ここから挿入操作 */
			if (newtbf->usec < diff_usec) {
				/* ここで差分をする(最後に挿入される以外は現在ノードの値も差分) */
				/* 差分のキュー現在ノードのタイマ値 - 現在までカウントしたタイマ値 - 新規作成したノードのタイマ値 */
				worktbf->usec = diff_usec - newtbf->usec;
				break;
    	}
			time_now = 0;
    	newtbf->usec -= diff_usec; /* 新規作成したノードのタイマ値 - 差分のキューの現在ノードのタイマ値 - 現在までカウントしたタイマ値  */
			tmptbf = worktbf;
			worktbf = worktbf->next;
		}
	
		/* ポインタの付け替え */
		newtbf->next = worktbf;
		/* 先頭に挿入 */
		if (i == 0) {
			DEBUG_OUTMSG("many1.\n");
			worktbf->prev = newtbf;
			mg_timerque.tmrhead = newtbf;
		}
		/* 最後に代入 */
		else if (worktbf == NULL) {
			DEBUG_OUTMSG("many2.\n");
			newtbf->prev = tmptbf;
			tmptbf->next = newtbf;
		}
		/* 上記以外に挿入 */
		else {
			DEBUG_OUTMSG("many3.\n");
			newtbf->prev = worktbf->prev;
			worktbf->prev = worktbf->prev->next = newtbf;
		}
		start_oneshot_timer(mg_timerque.index, mg_timerque.tmrhead->usec); /* 差分のキューの先頭のノードのタイマをスタートさせる */
	}
}


/*!
 * 差分のキューからノードを進める(古いノードは排除) 
 */
static void next_tmrcb_diffque(void)
{
	TMRCB *worktbf;

	worktbf = mg_timerque.tmrhead;

	/* 差分のキューに次ノードがある場合 */
	if (worktbf->next != NULL) {
		mg_timerque.tmrhead = worktbf->next; /* 次のタイマコントロールブロックへ */
		mg_timerque.tmrhead->prev = NULL;
		start_oneshot_timer(mg_timerque.index, mg_timerque.tmrhead->usec); /* タイマをスタートさせる */
	}
	/* 差分のキューに次ノードがない場合 */
	else {
		mg_timerque.tmrhead = NULL;
		DEBUG_OUTMSG("all release timercb2.\n");
	}
	/* タイマコントロールブロックの動的メモリ解放(カーネルが発行できるシステムコールはないため，内部関数を呼ぶ) */
	rel_mpf_isr(worktbf);
}


/*!
 * 差分のキューのノードを排除
 * *deltbf : 対象排除タイマコントロールブロック
 * (返却値)E_OK : 正常終了
 */
void delete_tmrcb_diffque(TMRCB *deltbf)
{

	/*
	* タイマキューの先頭を抜き取る
	* 一度タイマをキャンセルする(タイマコントロールブロックが排除されたからといって，
	* タイマ割込みが発生しなくなることはない.つまり，コールバックルーチンは呼ばれないが，
	* タイマ割込みが発生する.)
	* また，このタイマキャンセル処理は下のif文内のタイマ起動処理と近づけないと，ソフトタイマの
	* 誤差が蓄積されていっていしまう.
	*/
	if (deltbf == mg_timerque.tmrhead) {
		mg_timerque.tmrhead = deltbf->next;
		mg_timerque.tmrhead->prev = NULL;
		cancel_timer(1);
		/* まだタイマ要求があれば次の要求にうつる */
		if (mg_timerque.tmrhead != NULL) {
			start_oneshot_timer(mg_timerque.index, mg_timerque.tmrhead->usec); /* タイマをスタートさせる */
		}
	}
	/* タイマキューの最後から抜き取る */
	else if (deltbf->next == NULL) {
		deltbf->prev->next = NULL;
	}
	/*
	* タイマキューの中間から抜き取る
	* 差分のキューとしてソフトタイマを実装しているので，
	* 中間から抜き取る時は後続のノードに抜き取るノードの差分値を加算する
	*/
	else {
		deltbf->next->usec = deltbf->next->usec + deltbf->usec;
		deltbf->prev->next = deltbf->next;
		deltbf->next->prev = deltbf->prev;
	}
	/*
	* タイマコントロールブロックの動的メモリ解放
	* メモリ解放を行うので，deltbfのポインタはNULLしておかなくてよい
	*/
	rel_mpf_isr(deltbf);
}
