/*********************USAGE**********************************/
/* I) Prerequisite Information                              */
/*    Add #include "andebug.h" to where this file is used   */
/*    There are three Flags which feature PARALLELY         */
/* 1) To Enable Debug, Set _IsDebugEnabled To Non-Zero      */
/*    To Disable Debug, Set _IsDebugEnabled To Zero         */
/* 2) To Enable Assertion, Set _IsAsserted To Non-Zero      */
/*    To Disable Assertion, Set _IsAsserted To Zero         */
/*    When _IsAsserted = 0, both anDebug() and anAssert()   */
/*      do the same things no matter which given conditions */
/* 3) To Add Position Into Debug Printed-Out Messages       */
/*    To Enable This, Set _IsLocated To Non-Zero            */
/*    To Disable This, Set _IsLocated To Zero               */
/*II) THREE FUNCTIONS:                                      */
/*          anDebugWrap(), anDebug() and anAssert()         */
/*    FUNCTION anDebugWrap() stands for Debugging Wrapper   */
/*    Usage: anDebugWrap(<logical expression>, <code>) acts */
/*    as a switch to execute <code>, just simple as follows,*/
/*      when <logical expression> is TRUE, then run <code>  */
/*      when <logical expression> is FALSE, do nothing      */
/*    FUNCTION wdbg inherits from qDebug()                  */
/*    Usage: anDebug(<Input>) acts as qDebug() << <Input>;  */
/*      Ex: <Input> can be "ahjhj" << 5+8 << QBArr.toHex()  */
/*    FUNCTION adbg imitates standard method assert(),      */
/*      and also inherits from qDebug()                     */
/*    Usage:__With NON-ZERO _IsAsserted,                    */
/*          |anAssert(<logical expression>, <Input>) acts as*/
/*          |IF NOT <logical expression>                    */
/*          |       THEN anDebug(<Input>)                   */
/*          |,meaning only print <Input> if condition fails */
/*          __With ZERO _IsAsserted(),                      */
/*          |anAssert(<logical expression>, <Input>)        */
/*          |is identical to anDebug(<Input>)               */
/*          |, meaning always print <Input>                 */
/*III)STRAY FUNCTION: anqDebug() is here for a purpose of   */
/*      printing out exactly what you input without any mod-*/
/*      -ification by any flag, except for _IsDebugEnabled  */
/*    Usage: anqDebug(<Input>) equals to qDebug()<< <Input>;*/
/*IV) In the end, MISCELLANY section is in aid of this usage*/
/*********************EXAMPLES*******************************/
/*    Notation <10x> means _IsDebugEnabled Is Non-Zero      */
/*                         _IsAsserted Is Zero              */
/*                         _IsLocated Is Not Cared          */
/* 0) anqDebug("this is not modified if printed out"<<"OK");*/
/*    <0xx>=>""                                             */
/*               (All Debug Messages Are Globally Disabled) */
/*    <1xx>=>this is not modified if printed out OK         */
/* 1) anDebug("deb essage"<<4<<"infinite << can follow");   */
/*    <1x0>=>deb essage 4 infinite << can follow            */
/*    <1x1>=>deb essage 4 infinite << can follow <- int     */
/*                          qMain(int, char**) "main.cpp" 8 */
/*    After "<-" is the location of code anDebug(...) above */
/*     in a format : <function-name> <file-name> <line>     */
/* 2) anAssert(1<10, "this msg should not be printed out"); */
/*    <11x>=>""                                             */
/*                    (No Debug Message Cause 1<10 Is True) */
/*    <100>=>this msg should not be printed out             */
/*    <101>=>this msg should not be printed out <- int qMa- */
/*                            -in(int, char**) "main.cpp" 8 */
/* 3) anAssert((5>3)&&((1>2)||(4<3)),"debug msg"<<3<<"etc");*/
/*    <110>=>ASSERT Failed : (5>3)&&((1<2)||(4<3)) |  debug */
/*                                                msg 3 etc */
/*    <111>=>ASSERT Failed : (5>3)&&((1<2)||(4<3)) |  debug */
/*         msg 3 etc <- int qMain(int, char**) "main.cpp" 8 */
/*    <101>=>debug msg 3 etc <- int qMain(int, char**) "ma- */
/*                                               -in.cpp" 8 */
/*    <100>=>debug msg 3 etc                                */
/* 4) Providing a situation that debug messages from an in- */
/*      -dividual class (not all classes) are not necessary */
/*      , then here is a way to turn on/off those messages: */
/*      a) Below #define <CLASSNAME> in file <classname>.h, */
/*         insert #define UniqueKeyNameToTurnOn4ThisClass 1 */
/*                                 (Turn Off = Set To Zero) */
/*      b) Use anDebugWrap() in combination with functions  */
/*                  anqDebug(), anDebug() or anAssert()     */
/*         Ex:                                              */
/*          anDebugWrap(UniqueKeyNameToTurnOn4ThisClass,    */
/*          anAssert(4>9,"So Each Class Have Its Own Key"));*/
/************************************************************/
#ifndef ANDEBUG_H
#define ANDEBUG_H
/*********************DebugFlags*****************************/
//_IsDebugEnabled Is Zero => Debug Is Globally Disabled
#define _IsDebugEnabled 1
//_IsAsserted Is Non-Zero => Debug Is Additionally Conditioned
#define _IsAsserted 1
//_IsLocated Is Non-Zero => Debug Is Verbosely Located
#define _IsLocated 1
/*********************Function Definition********************/
#if _IsDebugEnabled
    #define anqDebug(...) qDebug() << __VA_ARGS__;
    #define anDebugWrap( LogicalExpression, ...)             \
        if (LogicalExpression) { __VA_ARGS__;}
    #define anDebug(...) qDebug() << __VA_ARGS__             \
                                    << "" anDebugLocation;
    #define anAssert( LogicalExpression, ...)                \
        IfNot(LogicalExpression)                             \
            anDebug("" AssHead(LogicalExpression)            \
                        << __VA_ARGS__)
#else
    #undef _IsAsserted
    #undef _IsLocated
    #define anqDebug(...)
    #define anDebugWrap( LogicalExpression, ...)
    #define anDebug(...)
    #define anAssert( LogicalExpression, ...)
#endif
/************************************************************/
#if _IsAsserted
    #define IfNot(LogicalExpression) if (!(LogicalExpression))
    #define AssHead(LogicalExpression) "ASSERT Failed : "    \
                    #LogicalExpression " | "
#else
    #define IfNot(LogicalExpression)
    #define AssHead(LogicalExpression)
#endif
#if _IsLocated
    #define anDebugLocation "<-"                             \
        << __PRETTY_FUNCTION__ << __FILENAME__ << __LINE__
#else
    #define anDebugLocation
#endif
/*********************Miscellany*****************************/
/* This piece of code supports these debug functionality,   */
/* however, apart from the main purpose, what are defined in*/
/* in this section can be used separately without any conce-*/
/*  -rns for how debug flags are set. So, just have to add  */
/*  #include "andebug.h" then use them in your own style    */
/************************************************************/
//__FILENAME__ gives back file-name without any relative path
//return type of QString
#ifdef _WIN32
    #define __FILENAME__ QString(__FILE__).split('\\'        \
                        , QString::SkipEmptyParts).last()
#else
    #define __FILENAME__ QString(__FILE__).split('/'         \
                        , QString::SkipEmptyParts).last()
#endif
//_VarView(VarName) stands for Variable View
//input directly <VarName>
//return type of QString formatted "<VarName>=<VarValue>"
#define _VarView(VarName) #VarName "="                       \
                            + QVariant(VarName).toString()
/************************************************************/
#include <QDebug>
#include <QVariant>
/************************************************************/
#endif // ANDEBUG_H
