/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Function prototypes.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#ifndef __PROTOS_H__
#define	__PROTOS_H__

#include <stdio.h>
#include "forth.h"
#include "cyberspc.h"
#include "mememat.h"
#include "rfile.h"
#include "meme.h"
#include "memenet.h"

/* allocdic.c */
void		allocate_memory(char **, char **, unsigned cell);
void		free_memory(char *);
void		allocate_dictionary(unsigned cell);
void		free_dictionary(void);

/* chario.c */
void		linemode(void);
void		keymode(void);
void		restoremode(void);
int			key(void);
void		emit(u_char c);
int			key_avail(void);

/* cyberspc.c */
void		DestroyRegion(Region *r);
cell		PointInRegion(Region *r, double x, double y, double z);
cell		PointInPrism(RPrism *rp, double x, double y, double z);
cell		OverlappingPrisms(RPrism *rp1, RPrism *rp2);
cell		OverlappingRegions(Region *r1, Region *r2);
Region * 	AddToRegion(Region *regionp, RPrism *prismp);
Region *	NextDBRegion(void);
Region *	FirstDBRegion(int);
Region *	AddDBRegion(Region *rp, int gflag);
Region *	DeleteDBRegion(Region *rp, int gflag);
Region *	RegisterDBRegion(Region *rp);
Region *	UnregisterDBRegion(Region *rp);
Region *	EnclosingDBRegion(double x, double y, double z);
Region *	FirstOverlappingDBRegion(Region *rp);
Region *	NextOverlappingDBRegion(Region *rp);

/* debuglog.c */
void		DebugLog(char *str);

/* decser.c */
int			DecryptSernum(char *plaintext, char *cryptext, cell *checksum);

/* dkmodule.c */
cell		RegisterDeckModule(module *mp);

/* expiry.c */
void		expired(void);
void		check_expiry(void);
void		cprint_evalcopymessage(void);

/* forth.c */
int			inner_interpreter(register token_t *ip);
char		*getaligned(u_cell nbytes);
void		init_dictionary(void);
void		init_variables(void);
void		init_entries(void);
void		fatal(char *str, int status);
void		read_dictionary(char *name);
void		udot(unsigned cell u);
void		dot(cell n);
void		type(u_char *adr, cell len);
token_t		*aligned(u_char *addr);
token_t		*name_from(u_char *nfa);
void		makeimmediate(void);
int			vfind(u_char **sp, vocabulary_t *voc);
int			find_local(u_char **sp);
int			find(cell *sp);
void		align(void);
void		comma_string(u_char *str);
void		quote_create(u_char *str, token_t cf);
void		hide(void);
void		reveal(void);
u_char		*blword();
u_char		*word(u_char delim);
void		scompile(u_char *str);
void		execute_one(token_t token);
void		execute_one_with_one_arg(token_t token, cell arg);
void		Xexecute_one_with_one_arg(token_t token, cell arg);
int			interpret_word(u_char *str);
void		where(void);
int			handle_literal(u_char *str);
void		query(void);
void		interpret(void);
void		interpret_line(void);
void		outer_interpreter(void);
int			number(u_char *str, long *n);
cell		digit(cell base, u_char c);
void		cmove(u_char *from, u_char *to, unsigned cell length);
void		cmove_up(u_char *from, u_char *to, unsigned cell length);
void		fill(u_char *to, u_cell length, u_char with);
char		*tocstr(u_char *from);
u_char		*tofstr(u_char *from);
u_char		*canonical(u_char *fstr);
cell		strindex(u_char *adr1, cell len1, u_char *adr2, cell len2);
void		pluslevel(void);
int			declevel(void);
char		*substr(char *str, int pos, int n);
void		tokstore(token_t token, cell *adr);
void		execute_set_relocation_bit(cell *adr);
void		set_relocation_bit(cell *adr);
cell		doccall(cell callno, u_char *format, cell **spp);
void main_set_relocation_bit(cell *adr);
void module_set_relocation_bit(cell *adr);
cell		doccall(cell callno, u_char *format, cell **spp);
int			isfliteral(unsigned char *str);
int			isfloatnum(unsigned char *str);

/* init.c */
void		MemeUsage(char *str);
void		ProcessCmdLineArgs(int *argcp, char *(*argvp[]), char *cmdline);
void		SetUpInterpreter(void);
void		SetUpEnv(void);
void		CleanUpEnv(void);
void		CleanUpMeme(void);
void		ExitMeme(int);

/* io.c */
void		title(void);
void		init_io(void);
void		emit(u_char c);
void		pop_all(void);
void		cprint(char *str);
void		warning(char *str);
void		error(char *str);
int			nextchar(int);
void		cexpect(u_char *addr, cell count);
void		load(u_char *filename);
void		file_error(char *str, char *filename);
void		set_input_file(char *str, FILE *file);
void		prompt(void);
void		load_pop(void);
int			key(void);
void		open_next_file(void);
cell		pfgetc(FILE *fp);
cell		pfgets(u_char *adr, register cell len, register FILE *f);
cell		pfgetline(u_char *adr, register FILE *f);

/* malloca.c */
char *		getdictmem(unsigned cell);
void		freedictmem(void);

/* meme.c */
void 		InitializeObject(Object *obj, cell type, cell subtype, cell linktype);
void		InitializeObjects(void);
Object *	FirstFreeObject(void);
cell		NumberObjects(void);
Object *	ParentOf(Object *obj);
cell		Orphan(Object *o);
cell		Adopts(Object *parent, Object *child);
Object *	NewObject(Object *parentobj, cell type, cell subtype,
							cell linktype);
void		DestroyObject(Object *obj);
Object *	NewImaginaryObject(Object *parent, cell linktype);
Object *	NewCompoundObject(Object *parent, cell linktype);
Object *	NewAudibleObject(Object *parent, cell linktype);
CModule *	FirstFreeModule(void);
CModule *	WhichModule(module *mp);
void *		NewModule(cell size);
void		DestroyModule(void *p);
void		Float2IFloat(double f, cell *exp, cell *man);
void		IFloat2Float(cell exp, cell man, double *f);

/* memejoy.c */
cell		MemeJoystickX(void);
cell		MemeJoystickY(void);
cell		MemeJoystickB(void);

/* mememat.c */
void		SetUpMaterials(void);
void		CleanUpMaterials(void);
void *		NewMat(void);
void		DestroyMat(void *mp);
int			ValidMat(mMat *mp);
void		MatColor(void *p, double r, double g, double b);
void		MatOpacity(void *p, double opacity);
void		MatSurface(void *p, double Ka, double Kd, double Ks, double power);
void		MatGeometry(void *p, cell g);
void		MatShading(void *p, cell s);
void		MatTexture(void *p, void *tex);
void		MatWrap(void *p, cell wrap);


/* mememaus.c */
void		SetUpMouse(void);
void		CleanUpMouse(void);
cell		MemeMouseX(void);
cell		MemeMouseY(void);
cell		MemeMouseB(void);

/* memenet.c */
#ifdef REMOVE
static MSock *	AllocMSock(void);
static MSock *	FreeMSock(MSock *msp);
#endif
void			SetUpNetwork(void);
void			NetTitle(void);
void			CleanUpNetwork(void);
cell			NetConnect(unsigned long host, cell port, cell timeout);
cell			NetListen(cell port, cell timeout);
cell			NetConnected(void *s);
cell			NetAccepted(void *s);
cell			NetStatus(void *v);
cell			NetRead(void *s, void *buffer, cell len);
cell			NetWrite(void *s, void *buffer, cell len);
cell			NetDisconnect(void *s, cell timeout);
cell			NetDisconnected(void *s);
void			NetOften(void);
HOSTID			NetName2HostID(char *name);
cell			IsMyHostID(HOSTID hostid);
HOSTID			SockMYIP(void *s);
cell			SockMyPort(void *s);
HOSTID			SockHerIP(void *s);
cell			SockHerPort(void *s);
cell			SendLocalMemeMessage(Message *msg, cell force);
void			SetUpSMsg(void);
void			CleanUpSMsg(void);
SMsg *			AllocSMsg(Message *m);
void			FreeSMsg(SMsg *p);
SMsg *			MsgToStruct(Message *);
cell			SendRemoteMemeMessage(Message *msg);
#ifdef REMOVE
void			DispatchIncomingMessages(MSock *);

#endif
cell			SendMemeMessage(void *, cell);
cell			NetDispatch(void *v, cell flags, cell timeoutms);
// void			DispatchAsyncMessage(UINT message, WPARAM wParam, LPARAM lParam);

/* memerend.c */
void		SetUpRendering(void *);
void		RenderingBanner(void);
void		CleanUpRendering(void);
void		_MemeSetCameraViewWindow(void *vcam, double xfactor, double yfactor);
void		_MemeGetCameraViewWindow(void *vcam, double *xfactor, double *yfactor);

Object 		*NewCameraObject(Object *parent, cell linktype, cell subtype, cell x, cell y,
													cell width, cell height);
Object 		*NewLightObject(Object *parent, cell linktype, cell subtype);
Object 		*NewVisibleObject(Object *parent, cell linktype, cell subtype, char *filename);
Object		*NewSpriteObject(Object *parent, cell linktype, cell texture);
Object		*NewImaginaryObject(Object *parent, cell linktype);
Object		*NewCompoundObject(Object *parent, cell linktype);
// Object	*NewAudibleObject(Object *parent, cell linktype);
void		EngineSpecificDestroyObject(Object *p);
void		TransformTree(Object *objarg, double pxscale, double pyscale, double pzscale);
void		AbsolutePosition(Object *obj, double *x, double *y, double *z);
void		TransformBranch(Object **lineage, RwMatrix4d *matrix,
							double pxscale, double pyscale, double pzscale);
cell		GetSubtreeMatrix(Object *obj, RwMatrix4d **mat);
cell		Transform(Object *obja, Object *objb, Object *result);
void		Render(void);
void		MemeCurrentCamera(Object *obj);
void		MemeSetCameraBackColor(Object *obj, double r, double g,
								double b);
static void	_MemeRemoveCameraBackdrop(Object *obj);
void		MemeSetCameraBackdrop(Object *obj, char *filename);
void		MemeSetCameraBackdropOffset(Object *obj, cell x, cell y);
void		MemeSetCameraViewport(Object *obj, cell x, cell y, cell w, cell h);
void		MemeSetCameraViewWindow(Object *obj, double w, double h);
cell		MemeScreenHeight(void);
cell		MemeScreenWidth(void);
cell		MemeWindowHeight(void);
cell		MemeWindowWidth(void);
void		MemeSetColor(Object *obj, double r, double g, double b, double opacity);
void		MemePickObject(MemePickRecord *mrec, cell screenx, cell screeny);
void		MemeSetVertex(Object *obj, cell vindex, double x,
							double y, double z);
cell		MemePolygonCount(Object *obj);
cell		MemeNewTexture(char *filename);
cell		MemeNewMaskedTexture(char *texfile, char *maskfile);
void		MemeTextureFrameStep(void *tp, cell n);
void		MemeTextureNextFrame(void *tp);
void		MemeSetTextureFrame(void *tp, cell n);
void		MemePolygonMaterial(void *pp, mMat *mp);
void		MemeShapeMaterial(void *objp, void *mp);
void		MemeGetPolygonMaterial(void *vp, void *mp);
void		MemeWriteShape(void *objp, char *filename);
void		MemeDestroyTexture(void *texturep);
cell		MemeGetVertexCount(Object *obj);
void		MemeGetVertex(Object *obj, cell index, double *x, double *y, double *z);
Object *	MemeWhichObject(void *p);
void		MemeLightColor(Object *obj, double r, double g, double b);
void		MemeLightConeAngle(Object *obj, double umbra, double penumbra);
void *		MemeGetPolygon(Object *obj, cell pindex);
void		MemeEmptyShape(Object *obj);
cell		MemeAddVertex(Object *obj, double x, double y, double z);
void *		MemeAddPolygon(Object *obj, cell vindexlist[], cell nsides);
void		MemeAddShape(Object *srcobj, Object *destobj);
void		MemeReversePolygonNormal(void *polyp);
void		MemeReverseShapeNormals(Object *obj);
void		MemeTextureCamera(Object *obj, cell texptr);

/* memeser.c */
void		SetUpSerial(void);
void		CleanUpSerial(void);
cell		SerialOpen(cell comport, cell baudrate, cell parity,
				cell word_length, cell stop_bits);
cell		SerialClose(cell portdesc);
cell		SerialRead(cell portdesc, cell buffer, cell len);
cell		SerialWrite(cell portdesc, cell buff, cell len);

/* often.c */
void		MemeOften(void);

/* rfcache.c */
void		SetUpRFCache(void);
void		CleanUpRFCache(void);
void		AddRFileToCache(char *urlname, char *tmpname);
void		IsCached(char *urlname, char *tmpname);

/* rfile.c */
void		SetUpRFiles(void);
void		CleanUpRFiles(void);
void *		AllocRFile(void);
void		FreeRFile(void *);
void		Name2URL(char *name, URL *up);
cell		MemeRFileRequest(cell hostid, char *urlname, cell port);
cell		HttpRFileRequest(cell hostid, char *pathname, cell port);
void		OpenURL(char *urlname, RFRequest *req, cell type);
void		WaitURL(RFRequest *req);
void		HandleFileServerMessage(Message *msg);
void		ParseHTTPHeader(char *hdr, cell *errcode, cell *filelen);
void		RFileOften(void);


/* syscall.c */
cell		dosyscall(cell callno, u_char *format, cell **spp);
void		handle_signal(int sig, int code);
void		init_signals(void);
void		report_signal(void);

/* util.c */
uint32		myrand(void);
void		myrandomize(void);
cell		FileSize(char *name);
void		Memetmpname(char *p, cell tag);
void		PathName(char *file, char *dest, char *listp);
void		MemeFilePathName(char *file, char *dest);
void		safestrcpy(char *dest, char *src, int maxlen);
int			abspathname(char *s);
char *		CurrentYear(void);

#endif
