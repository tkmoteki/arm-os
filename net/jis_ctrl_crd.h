#ifndef _JIS_CTRL_CRD_H_INCLUDED_
#define _JIS_CTRL_CRD_H_INCLUDED_


/* 制御コード定義(JIS X 0211~C0集合) */

#define JIS_X_0211_NUL	0x00	/*! NULL */
#define JIS_X_0211_SOH	0x01	/*! ヘッダ開始(xmodemは，送信開始) */
#define JIS_X_0211_STX	0x02	/*! テキスト開始 */
#define JIS_X_0211_ETX	0x03	/*! テキスト終了 */
#define JIS_X_0211_EOT	0x04	/*! 転送終了 */
#define JIS_X_0211_ENQ	0x05	/*! 問い合わせ */
#define JIS_X_0211_ACK	0x06	/*! 肯定応答 */
#define JIS_X_0211_BEL	0x07	/*! ベル */
#define JIS_X_0211_BS		0x08	/*! 後退 */
#define JIS_X_0211_HT		0x09	/*! 水平タブ */
#define JIS_X_0211_LF		0x0a	/*! 改行 */
#define JIS_X_0211_VT		0x0B	/*! 垂直タブ */
#define JIS_X_0211_FF		0x0C	/*! 改ページ */
#define JIS_X_0211_CR		0x0d	/*! 復帰(xmodemは，EOT後にブロックを送信した等) */
#define JIS_X_0211_SO		0x0e	/*! シフトアウト */
#define JIS_X_0211_SI		0x0f	/*! シフトイン */
#define JIS_X_0211_DLE	0x10	/*! 伝送制御拡張 */
#define JIS_X_0211_DC1	0x11	/*! その他1(装置制御) */
#define JIS_X_0211_DC2	0x12	/*! その他2(装置制御) */
#define JIS_X_0211_DC3	0x13	/*! その他3(装置制御) */
#define JIS_X_0211_DC4	0x14	/*! その他4(装置制御) */
#define JIS_X_0211_NAK	0x15	/*! 否定応答 */
#define JIS_X_0211_SYN	0x16	/*! 同期信号 */
#define JIS_X_0211_ETB	0x17	/*! 転送ブロック終了 */
#define JIS_X_0211_CAN	0x18	/*! 中断 */
#define JIS_X_0211_EM		0x19	/*! 媒体終端 */
#define JIS_X_0211_SUB	0x1a	/*! 置換(xmodemは，ブロック内の足りないデータを埋めるのに使用) */
#define JIS_X_0211_ESC	0x1b	/*! 拡張 */
#define JIS_X_0211_FS		0x1c	/*! ファイル分割 */
#define JIS_X_0211_GS		0x1d	/*! グループ分割 */
#define JIS_X_0211_RS		0x1e	/*! レコード分割 */
#define JIS_X_0211_US		ox1f	/*! ユニット分割 */


#endif
