/*! \file levels.cpp
 * \brief Reality levels stuff.
 *
 * $Id$
 *
 * See mux/REALITY.SETUP in the distribution.
 */

#ifdef REALITY_LVLS

#include "copyright.h"
#include "autoconf.h"
#include "config.h"
#include "externs.h"

#include "attrs.h"
#include "command.h"
#include "powers.h"
#include "levels.h"

RLEVEL RxLevel(dbref thing)
{
    const UTF8 *buff = atr_get_raw(thing, A_RLEVEL);
    if (  NULL == buff
       || strlen((char *)buff) != 17)
    {
        switch(Typeof(thing))
        {
        case TYPE_ROOM:
            return(mudconf.def_room_rx);

        case TYPE_PLAYER:
            return(mudconf.def_player_rx);

        case TYPE_EXIT:
            return(mudconf.def_exit_rx);

        default:
            return(mudconf.def_thing_rx);
        }
    }

    int i;
    RLEVEL rx = 0;
    for (i = 0; mux_isxdigit(buff[i]); i++)
    {
        rx = 16 * rx + mux_hex2dec(buff[i]);
    }
    return rx;
}

RLEVEL TxLevel(dbref thing)
{
    const UTF8 *buff = atr_get_raw(thing, A_RLEVEL);
    if (  NULL == buff
       || strlen((char *)buff) != 17)
    {
        switch(Typeof(thing))
        {
        case TYPE_ROOM:
            return(mudconf.def_room_tx);

        case TYPE_PLAYER:
            return(mudconf.def_player_tx);

        case TYPE_EXIT:
            return(mudconf.def_exit_tx);

        default:
            return(mudconf.def_thing_tx);
        }
    }

    // Skip the first field.
    //
    int i;
    for (i = 0; buff[i] && !mux_isspace(buff[i]); i++)
    {
        ; // Nothing.
    }

    RLEVEL tx = 0;
    if (buff[i])
    {
        // Skip space found above.
        //
        i++;

        // Decode second field.
        //
        for ( ; mux_isxdigit(buff[i]); i++)
        {
            tx = 16 * tx + mux_hex2dec(buff[i]);
        }
    }
    return tx;
}

void notify_except_rlevel
(
    dbref loc,
    dbref player,
    dbref exception,
    const UTF8 *msg,
    int xflags
)
{
    if (  loc != exception
       && IsReal(loc, player))
    {
        notify_check(loc, player, msg,
            (MSG_ME_ALL | MSG_F_UP | MSG_S_INSIDE | MSG_NBR_EXITS_A| xflags));
    }

    dbref first;
    DOLIST(first, Contents(loc))
    {
        if (  first != exception
           && IsReal(first, player))
        {
            notify_check(first, player, msg,
                (MSG_ME | MSG_F_DOWN | MSG_S_OUTSIDE | xflags));
        }
    }
}

void notify_except2_rlevel
(
    dbref loc,
    dbref player,
    dbref exc1,
    dbref exc2,
    const UTF8 *msg
)
{
    if (  loc != exc1
       && loc != exc2
       && IsReal(loc, player))
    {
        notify_check(loc, player, msg,
            (MSG_ME_ALL | MSG_F_UP | MSG_S_INSIDE | MSG_NBR_EXITS_A));
    }

    dbref first;
    DOLIST(first, Contents(loc))
    {
        if (  first != exc1
           && first != exc2
           && IsReal(first, player))
        {
            notify_check(first, player, msg,
                (MSG_ME | MSG_F_DOWN | MSG_S_OUTSIDE));
        }
    }
}

void notify_except2_rlevel2
(
    dbref loc,
    dbref player,
    dbref exc1,
    dbref exc2,
    const UTF8 *msg
)
{
    if (  loc != exc1
       && loc != exc2
       && IsReal(loc, player))
    {
        notify_check(loc, player, msg,
            (MSG_ME_ALL | MSG_F_UP | MSG_S_INSIDE | MSG_NBR_EXITS_A));
    }

    dbref first;
    DOLIST(first, Contents(loc))
    {
        if (  first != exc1
           && first != exc2
           && IsReal(first, player)
           && IsReal(first, exc2))
        {
            notify_check(first, player, msg,
                (MSG_ME | MSG_F_DOWN | MSG_S_OUTSIDE));
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * * rxlevel_description: Return an mbuf containing the RxLevels of the thing.
 */

UTF8 *rxlevel_description(dbref player, dbref target)
{
    // Allocate the return buffer.
    //
    int  otype = Typeof(target);
    UTF8 *buff = alloc_mbuf("rxlevel_description");
    UTF8 *bp   = buff;

    int i;
    RLEVEL rl = RxLevel(target);
    for (i = 0; i < mudconf.no_levels; ++i)
    {
        if (  (rl & mudconf.reality_level[i].value)
           == mudconf.reality_level[i].value)
        {
            safe_mb_chr(' ', buff, &bp);
            safe_mb_str(mudconf.reality_level[i].name, buff, &bp);
        }
    }

    // Terminate the string, and return the buffer to the caller.
    //
    *bp = '\0';
    return buff;
}

/*
 * ---------------------------------------------------------------------------
 * * txlevel_description: Return an mbuf containing the TxLevels of the thing.
 */

UTF8 *txlevel_description(dbref player, dbref target)
{
    // Allocate the return buffer.
    //
    int otype = Typeof(target);
    UTF8 *buff = alloc_mbuf("txlevel_description");
    UTF8 *bp = buff;

    int i;
    RLEVEL tl = TxLevel(target);
    for (i = 0; i < mudconf.no_levels; ++i)
    {
        if (  (tl & mudconf.reality_level[i].value)
           == mudconf.reality_level[i].value)
        {
            safe_mb_chr(' ', buff, &bp);
            safe_mb_str(mudconf.reality_level[i].name, buff, &bp);
        }
    }

    // Terminate the string, and return the buffer to the caller.
    //
    *bp = '\0';
    return buff;
}

RLEVEL find_rlevel(UTF8 *name)
{
    for (int i = 0; i < mudconf.no_levels; i++)
    {
        if (mux_stricmp(name, mudconf.reality_level[i].name) == 0)
        {
            return mudconf.reality_level[i].value;
        }
    }
    return 0;
}

void do_rxlevel
(
    dbref player,
    dbref cause,
    dbref enactor,
    int   nargs,
    int   key,
    UTF8 *object,
    UTF8 *arg
)
{
    if (!arg || !*arg)
    {
        notify_quiet(player, T("I don't know what you want to set!"));
        return;
    }

    // Find thing.
    //
    dbref thing = match_controlled(player, object);
    if (NOTHING == thing)
    {
        return;
    }

    UTF8 lname[9];
    RLEVEL ormask = 0;
    RLEVEL andmask = ~ormask;
    while (*arg)
    {
        int negate = 0;
        while (  *arg != '\0'
              && mux_isspace(*arg))
        {
            arg++;
        }

        if (*arg == '!')
        {
            negate = 1;
            ++arg;
        }

        int i;
        for (i = 0; *arg && !mux_isspace(*arg); arg++)
        {
            if (i < 8)
            {
                lname[i++] = *arg;
            }
        }

        lname[i] = '\0';
        if (!lname[0])
        {
            if (negate)
            {
                notify(player, T("You must specify a reality level to clear."));
            }
            else
            {
                notify(player, T("You must specify a reality level to set."));
            }
            return;
        }

        RLEVEL result = find_rlevel(lname);
        if (!result)
        {
            notify(player, T("No such reality level."));
            continue;
        }
        if (negate)
        {
            andmask &= ~result;
            notify(player, T("Cleared."));
        }
        else
        {
            ormask |= result;
            notify(player, T("Set."));
        }
    }

    // Set the Rx Level.
    //
    UTF8 *buff = alloc_lbuf("do_rxlevel");
    mux_sprintf(buff, LBUF_SIZE, "%08X %08X", RxLevel(thing) & andmask | ormask, TxLevel(thing));
    atr_add_raw(thing, A_RLEVEL, buff);
    free_lbuf(buff);
}

void do_txlevel
(
    dbref player,
    dbref cause,
    dbref enactor,
    int nargs,
    int key,
    UTF8 *object,
    UTF8 *arg
)
{
    if (!arg || !*arg)
    {
        notify_quiet(player, T("I don't know what you want to set!"));
        return;
    }

    // Find thing.
    //
    dbref thing = match_controlled(player, object);
    if (NOTHING == thing)
    {
        return;
    }

    UTF8 lname[9];
    RLEVEL ormask = 0;
    RLEVEL andmask = ~ormask;
    while (*arg)
    {
        int negate = 0;
        while (  *arg
              && mux_isspace(*arg))
        {
            arg++;
        }

        if (*arg == '!')
        {
            negate = 1;
            ++arg;
        }

        int i;
        for (i = 0; *arg && !mux_isspace(*arg); arg++)
        {
            if (i < 8)
            {
                lname[i++] = *arg;
            }
        }

        lname[i] = '\0';
        if (!lname[0])
        {
            if (negate)
            {
                notify(player, T("You must specify a reality level to clear."));
            }
            else
            {
                notify(player, T("You must specify a reality level to set."));
            }
            return;
        }

        RLEVEL result = find_rlevel(lname);
        if (!result)
        {
            notify(player, T("No such reality level."));
            continue;
        }
        if (negate)
        {
            andmask &= ~result;
            notify(player, T("Cleared."));
        }
        else
        {
            ormask |= result;
            notify(player, T("Set."));
        }
    }

    // Set the Tx Level.
    //
    UTF8 *buff = alloc_lbuf("do_rxlevel");
    mux_sprintf(buff, LBUF_SIZE, "%08X %08X", RxLevel(thing), TxLevel(thing) & andmask | ormask);
    atr_add_raw(thing, A_RLEVEL, buff);
    free_lbuf(buff);
}

/*
 * ---------------------------------------------------------------------------
 * * decompile_rlevels: Produce commands to set reality levels on target.
 */

void decompile_rlevels(dbref player, dbref thing, UTF8 *thingname)
{
    UTF8 *buf = rxlevel_description(player, thing);
    notify(player, tprintf("@rxlevel %s=%s", thingname, buf));
    free_mbuf(buf);

    buf = txlevel_description(player, thing);
    notify(player, tprintf("@txlevel %s=%s", thingname, buf));
    free_mbuf(buf);
}

int *desclist_match(dbref player, dbref thing)
{
    static int descbuffer[33];

    descbuffer[0] = 1;
    RLEVEL match = RxLevel(player) & TxLevel(thing);
    for (int i = 0; i < mudconf.no_levels; i++)
    {
        if (  (match & mudconf.reality_level[i].value)
           == mudconf.reality_level[i].value)
        {
            ATTR *at = atr_str(mudconf.reality_level[i].attr);
            if (at)
            {
                int j;
                for (j = 1; j < descbuffer[0]; j++)
                {
                    if (at->number == descbuffer[j])
                    {
                        break;
                    }
                }
                if (j == descbuffer[0])
                {
                    descbuffer[descbuffer[0]++] = at->number;
                }
            }
        }
    }
    return descbuffer;
}

/* ---------------------------------------------------------------------------
 * did_it_rlevel: Have player do something to/with thing, watching the
 * attributes. 'what' is actually ignored, the desclist match being used
 * instead.
 */
void did_it_rlevel
(
    dbref player,
    dbref thing,
    int   what,
    const UTF8 *def,
    int   owhat,
    const UTF8 *odef,
    int   awhat,
    int   ctrl_flags,
    UTF8 *args[],
    int   nargs
)
{
    UTF8 *d, *buff, *act, *charges, *bp, *str;
    dbref loc, aowner;
    int num, aflags;
    int i, *desclist, found_a_desc;

    reg_ref **preserve = NULL;
    bool need_pres = false;

    // Message to player.
    //
    if (what > 0)
    {
        // Get description list.
        //
        desclist = desclist_match(player, thing);
        found_a_desc = 0;
        for (i = 1; i < desclist[0]; i++)
        {
            // Ok, if it's A_DESC, we need to check against A_IDESC.
            //
            if (  A_IDESC == what
               && A_DESC == desclist[i])
            {
                d = atr_pget(thing, A_IDESC, &aowner, &aflags);
            }
            else
            {
                d = atr_pget(thing, desclist[i], &aowner, &aflags);
            }

            if (*d)
            {
                // No need for the 'def' message.
                //
                found_a_desc = 1;
                if (!need_pres)
                {
                    need_pres = true;
                    preserve = PushRegisters(MAX_GLOBAL_REGS);
                    save_global_regs(preserve);
                }
                buff = bp = alloc_lbuf("did_it.1");
                mux_exec(d, buff, &bp, thing, thing, player,
                    AttrTrace(aflags, EV_EVAL|EV_FIGNORE|EV_TOP),
                    args, nargs);
                *bp = '\0';

                if (  A_HTDESC == desclist[i]
                   && Html(player))
                {
                    safe_str(T("\r\n"), buff, &bp);
                    *bp = '\0';
                    notify_html(player, buff);
                }
                else
                {
                    notify(player, buff);
                }
                free_lbuf(buff);
            }
            free_lbuf(d);
        }

        if (!found_a_desc)
        {
            // No desc found... try the default desc (again).
            // A_DESC or A_HTDESC... the worst case we look for it twice.
            //
            d = atr_pget(thing, what, &aowner, &aflags);
            if (*d)
            {
                // No need for the 'def' message
                //
                found_a_desc = 1;
                if (!need_pres)
                {
                    need_pres = true;
                    preserve = PushRegisters(MAX_GLOBAL_REGS);
                    save_global_regs(preserve);
                }
                buff = bp = alloc_lbuf("did_it.1");
                mux_exec(d, buff, &bp, thing, thing, player,
                    AttrTrace(aflags, EV_EVAL|EV_FIGNORE|EV_TOP),
                    args, nargs);
                *bp = '\0';

                if (  A_HTDESC == what
                   && Html(player))
                {
                    safe_str(T("\r\n"), buff, &bp);
                    *bp = '\0';
                    notify_html(player, buff);
                }
                else
                {
                    notify(player, buff);
                }
                free_lbuf(buff);
            }
            else if (def)
            {
                notify(player, def);
            }
            free_lbuf(d);
        }
    }
    else if (  what < 0
            && def)
    {
        notify(player, def);
    }

    if (isPlayer(thing))
    {
       d = atr_pget(mudconf.master_room, get_atr(T("ASSET_DESC")), &aowner, &aflags);
       if (*d)
       {
          if (!need_pres)
          {
             need_pres = true;
             preserve = PushRegisters(MAX_GLOBAL_REGS);
             save_global_regs(preserve);
          }
          buff = bp = alloc_lbuf("did_it.1");
          mux_exec(d, buff, &bp, thing, thing, player,
              AttrTrace(aflags, EV_EVAL|EV_FIGNORE|EV_TOP),
              args, nargs);
          *bp = '\0';
          notify(player, buff);
          free_lbuf(buff);
       }
       free_lbuf(d);
    }


    // Message to neighbors.
    //
    if (  0 < owhat
       && Has_location(player)
       && Good_obj(loc = Location(player)))
    {
        d = atr_pget(thing, owhat, &aowner, &aflags);
        if (*d)
        {
            if (!need_pres)
            {
                need_pres = true;
                preserve = PushRegisters(MAX_GLOBAL_REGS);
                save_global_regs(preserve);
            }
            buff = bp = alloc_lbuf("did_it.2");
            mux_exec(d, buff, &bp, thing, thing, player,
                AttrTrace(aflags, EV_EVAL|EV_FIGNORE|EV_TOP),
                args, nargs);
            *bp = '\0';

            if (*buff)
            {
                if (aflags & AF_NONAME)
                {
                    notify_except2_rlevel2(loc, player, player, thing, buff);
                }
                else
                {
                    notify_except2_rlevel2(loc, player, player, thing,
                        tprintf("%s %s", Name(player), buff));
                }
            }
            free_lbuf(buff);
        }
        else if (odef)
        {
            if (ctrl_flags & VERB_NONAME)
            {
                notify_except2_rlevel2(loc, player, player, thing, odef);
            }
            else
            {
                notify_except2_rlevel2(loc, player, player, thing,
                    tprintf("%s %s", Name(player), odef));
            }
        }
        free_lbuf(d);
    }
    else if (  owhat < 0
            && odef
            && Has_location(player)
            && Good_obj(loc = Location(player)))
    {
        if (ctrl_flags & VERB_NONAME)
        {
            notify_except2_rlevel2(loc, player, player, thing, odef);
        }
        else
        {
            notify_except2_rlevel2(loc, player, player, thing,
                tprintf("%s %s", Name(player), odef));
        }
    }

    // If we preserved the state of the global registers, restore them.
    //
    if (need_pres)
    {
        restore_global_regs(preserve);
        PopRegisters(preserve, MAX_GLOBAL_REGS);
    }

    // Do the action attribute.
    //
    if (  awhat > 0
       && IsReal(thing, player))
    {
        act = atr_pget(thing, awhat, &aowner, &aflags);
        if (*act != '\0')
        {
            charges = atr_pget(thing, A_CHARGES, &aowner, &aflags);
            if (*charges)
            {
                num = mux_atol(charges);
                if (num > 0)
                {
                    buff = alloc_sbuf("did_it.charges");
                    mux_ltoa(num-1, buff);
                    atr_add_raw(thing, A_CHARGES, buff);
                    free_sbuf(buff);
                }
                else
                {
                    buff = atr_pget(thing, A_RUNOUT, &aowner, &aflags);
                    if (*buff != '\0')
                    {
                        free_lbuf(act);
                        act = buff;
                    }
                    else
                    {
                        free_lbuf(act);
                        free_lbuf(buff);
                        free_lbuf(charges);
                        return;
                    }
                }
            }
            free_lbuf(charges);

            CLinearTimeAbsolute lta;
            wait_que(thing, player, player, AttrTrace(aflags, 0), false, lta,
                NOTHING, 0,
                act,
                nargs, args,
                mudstate.global_regs);
        }
        free_lbuf(act);
    }
}
#endif // REALITY_LVLS
