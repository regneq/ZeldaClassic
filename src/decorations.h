//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  sprite.cc
//
//  Sprite classes:
//   - sprite:      base class for the guys and enemies in zelda.cc
//   - movingblock: the moving block class
//   - sprite_list: main container class for different groups of sprites
//   - item:        items class
//
//--------------------------------------------------------

#ifndef _DECORATIONS_H_
#define _DECORATIONS_H_

#include "zfix.h"
/***************************************/
/*******  Decoration Base Class  *******/
/***************************************/

class decoration : public sprite
{
public:
	decoration(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	int the_deco_sprite; 
	virtual ~decoration();
};

/*******************************/
/*******   Decorations   *******/
/*******************************/

class comboSprite : public decoration
{
public:
    int timer;
    bool initialized;
	int tframes, spd;
    comboSprite(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
    virtual bool animate(int index);
    virtual void draw(BITMAP *dest);
    virtual void draw2(BITMAP *dest);
	virtual void realdraw(BITMAP *dest, int draw_what);
	
};

class dBushLeaves : public decoration
{
public:
	static int ft[4][8][3]; //[leaf][frame][x/y/flip]
	int ox, oy;
	dBushLeaves(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dFlowerClippings : public decoration
{
public:
	static int ft[4][8][3]; //[leaf][frame][x/y/flip]
	int ox, oy;
	dFlowerClippings(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dGrassClippings : public decoration
{
public:
	static int ft[3][4][4]; //[leaf][frame][x/y/flip/size(0=large, 1=small)]
	int ox, oy;
	dGrassClippings(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dHammerSmack : public decoration
{
public:
	static int ft[2][4][3]; //[leaf][frame][x/y/icon(0=spark, 1=star)]
	int ox, oy, wpnid;
	dHammerSmack(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dTallGrass : public decoration
{
public:
	dTallGrass(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dRipples : public decoration
{
public:
	dRipples(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dHover : public decoration
{
	int wpnid;
public:
	dHover(zfix X,zfix Y,int Id,int Clk, int wpnSpr=0);
	virtual bool animate(int index);
	virtual void draw(BITMAP *dest);
};

class dNayrusLoveShield : public decoration
{
public:
	dNayrusLoveShield(zfix X,zfix Y,int Id,int Clk);
	virtual bool animate(int index);
	virtual void realdraw(BITMAP *dest, int draw_what);
	virtual void draw(BITMAP *dest);
	virtual void draw2(BITMAP *dest);
};
#endif
/*** end of sprite.cc ***/

