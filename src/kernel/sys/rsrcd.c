# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <type.h>


mapping resources;	/* registered resources */
mapping owners;		/* resource owners */
mapping olimits;	/* resource limit per owner */
int downtime;		/* shutdown time */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize resource mappings
 */
static void create()
{
    /* initial resources */
    resources = ([
      "objects" :	({ -1,  0,    0 }),
      "stack" :		({ -1,  0,    0 }),
      "ticks" :		({ -1,  0,    0 }),
      "tick usage" :	({ -1, 10, 3600 }),
      "fileblocks" :	({ -1,  0,    0 }),
    ]);

    owners = ([ ]);		/* no resource owners yet */
    olimits = ([ ]);
}

/*
 * NAME:	add_owner()
 * DESCRIPTION:	add a new resource owner
 */
void add_owner(string owner)
{
    if (KERNEL() && !owners[owner]) {
	object obj;

	rlimits (-1; -1) {
	    obj = clone_object(RSRCOBJ);
	    catch {
		owners[owner] = obj;
		obj->set_owner(owner);
		owners["System"]->rsrc_incr("objects", 1, resources["objects"]);
		olimits[owner] = ({ -1, -1, 0 });
	    } : {
		destruct_object(obj);
		error("Too many resource owners");
	    }
	}
    }
}

/*
 * NAME:	remove_owner()
 * DESCRIPTION:	remove a resource owner
 */
void remove_owner(string owner)
{
    object obj;
    string *names;
    mixed **rsrcs, *rsrc;
    int i;

    if (previous_program() == API_RSRC && (obj=owners[owner])) {
	names = map_indices(resources);
	rsrcs = map_values(resources);
	for (i = sizeof(names); --i >= 0; ) {
	    rsrc = obj->rsrc_get(names[i], rsrcs[i]);
	    if (rsrc[RSRC_DECAY] == 0 && (int) rsrc[RSRC_USAGE] != 0) {
		error("Removing owner with non-zero resources");
	    }
	}

	rlimits (-1; -1) {
	    destruct_object(obj);
	    olimits[owner] = nil;
	}
    }
}

/*
 * NAME:	query_owners()
 * DESCRIPTION:	return a list of resource owners
 */
string *query_owners()
{
    if (previous_program() == API_RSRC) {
	return map_indices(owners);
    }
}


/*
 * NAME:	set_rsrc()
 * DESCRIPTION:	set the maximum, decay percentage and decay period of a
 *		resource
 */
void set_rsrc(string name, int max, int decay, int period)
{
    if (KERNEL()) {
	mixed *rsrc;

	rsrc = resources[name];
	if (rsrc) {
	    /*
	     * existing resource
	     */
	    if ((rsrc[GRSRC_DECAY] == 0) != (decay == 0)) {
		error("Cannot change resource decay");
	    }
	    rlimits (-1; -1) {
		rsrc[GRSRC_MAX] = max;
		rsrc[GRSRC_DECAY] = decay;
		rsrc[GRSRC_PERIOD] = period;
	    }
	} else {
	    /* new resource */
	    resources[name] = ({ max, decay, period });
	}
    }
}

/*
 * NAME:	remove_rsrc()
 * DESCRIPTION:	remove a resource
 */
void remove_rsrc(string name)
{
    object *objects;
    int i;

    if (previous_program() == API_RSRC) {
	if (!resources[name]) {
	    error("No such resource: " + name);
	}
	objects = map_values(owners);
	i = sizeof(objects);
	rlimits (-1; -1) {
	    while (i != 0) {
		objects[--i]->remove_rsrc(name);
	    }
	    resources[name] = nil;
	}
    }
}

/*
 * NAME:	query_rsrc()
 * DESCRIPTION:	get usage and limits of a resource
 */
mixed *query_rsrc(string name)
{
    if (previous_program() == API_RSRC) {
	mixed *rsrc, usage;
	object *objects;
	int i;

	if (!(rsrc=resources[name])) {
	    error("No such resource: " + name);
	}
	objects = map_values(owners);
	usage = (rsrc[GRSRC_DECAY] == 0) ? 0 : 0.0;
	for (i = sizeof(objects); --i >= 0; ) {
	    usage += objects[i]->rsrc_get(name, rsrc)[RSRC_USAGE];
	}

	return ({ usage, rsrc[GRSRC_MAX], 0 }) +
	       rsrc[GRSRC_DECAY .. GRSRC_PERIOD];
    }
}

/*
 * NAME:	query_resources()
 * DESCRIPTION:	return a list of resources
 */
string *query_resources()
{
    if (previous_program() == API_RSRC) {
	return map_indices(resources);
    }
}


/*
 * NAME:	rsrc_set_limit()
 * DESCRIPTION:	set individual resource limit
 */
void rsrc_set_limit(string owner, string name, int max)
{
    if (previous_program() == API_RSRC) {
	object obj;
	mixed *rsrc;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}
	if (!(rsrc=resources[name])) {
	    error("No such resource: " + name);
	}
	obj->rsrc_set_limit(name, max, rsrc[GRSRC_DECAY]);
    }
}

/*
 * NAME:	rsrc_get()
 * DESCRIPTION:	get individual resource usage
 */
mixed *rsrc_get(string owner, string name)
{
    if (KERNEL()) {
	object obj;
	mixed *rsrc;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}
	if (!(rsrc=resources[name])) {
	    error("No such resource: " + name);
	}
	return obj->rsrc_get(name, rsrc);
    }
}

/*
 * NAME:	rsrc_incr()
 * DESCRIPTION:	increment or decrement a resource
 */
void rsrc_incr(string owner, string name, int incr)
{
    if (KERNEL()) {
	object obj;
	mixed *rsrc;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}
	if (!(rsrc=resources[name])) {
	    error("No such resource: " + name);
	}
	obj->rsrc_incr(name, incr, rsrc);
    }
}

/*
 * NAME:	call_limits()
 * DESCRIPTION:	handle stack and tick limits for _F_call_limited
 */
mixed *call_limits(mixed *previous, string owner, int stack, int ticks)
{
    if (previous_program() == AUTO) {
	int maxstack, maxticks, time, *limits;

	limits = olimits[owner];

	/* determine available stack */
	maxstack = limits[LIM_MAX_STACK];
	if (maxstack < 0) {
	    maxstack = resources["stack"][GRSRC_MAX];
	}
	if (maxstack > stack && stack >= 0) {
	    maxstack = stack;
	}
	if (maxstack >= 0) {
	    maxstack++;
	}

	/* determine available ticks */
	maxticks = limits[LIM_MAX_TICKS];
	if (maxticks < 0) {
	    maxticks = resources["ticks"][GRSRC_MAX];
	} else {
	    int *usage;

	    usage = resources["tick usage"];
	    if ((time=time()) - limits[LIM_MAX_TIME] >= usage[GRSRC_PERIOD]) {
		/* decay ticks */
		owners[owner]->decay_ticks(limits, time, usage);
		maxticks = limits[LIM_MAX_TICKS];
	    }
	}
	if (maxticks > ticks - 25 && ticks >= 0) {
	    maxticks = ticks - 25;
	    if (maxticks <= 0) {
		maxticks = 1;
	    }
	}

	return ({ previous, owner, maxstack, maxticks, ticks });
    }
}

/*
 * NAME:	set_rlimits()
 * DESCRIPTION:	set limits for call_limited
 */
void set_rlimits(string owner, int *limits)
{
    if (previous_program() == RSRCOBJ) {
    	olimits[owner] = limits;
    }
}

/*
 * NAME:	update_ticks()
 * DESCRIPTION:	update ticks after execution
 */
int update_ticks(mixed *limits, int ticks)
{
    if (KERNEL()) {
	if (limits[LIM_MAXTICKS] > 0 &&
	    (!limits[LIM_NEXT] ||
	     limits[LIM_OWNER] != limits[LIM_NEXT][LIM_OWNER])) {
	    ticks = limits[LIM_MAXTICKS] - ticks;
	    if (ticks < 0) {
		return -1;
	    }
	    owners[limits[LIM_OWNER]]->update_ticks(ticks,
						    resources["tick usage"]);
	    ticks = (limits[LIM_NEXT] && limits[LIM_TICKS] >= 0) ?
		     limits[LIM_NEXT][LIM_MAXTICKS] -= ticks : -1;
	}
	return ticks;
    }
}


/*
 * NAME:	initd()
 * DESCRIPTION:	perform local system initialization
 */
object initd()
{
    if (previous_program() == DRIVER) {
	return compile_object("/usr/System/initd");
    }
}

/*
 * NAME:	prepare_reboot()
 * DESCRIPTION:	prepare for a reboot
 */
void prepare_reboot()
{
    if (previous_program() == DRIVER) {
	downtime = time();
    }
}

/*
 * NAME:	reboot()
 * DESCRIPTION:	recover from a reboot
 */
void reboot()
{
    if (previous_program() == DRIVER) {
	object *objects;
	int i;

	downtime = time() - downtime;
	objects = map_values(owners);
	for (i = sizeof(objects); --i >= 0; ) {
	    objects[i]->reboot(downtime);
	}
    }
}
