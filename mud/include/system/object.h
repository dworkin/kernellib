# define OBJECTD     "/usr/System/sys/objectd"     /* object manager */
# define OWNEROBJ    "/usr/System/obj/owner"       /* per-owner management */
# define API_OBJECT  "/usr/System/lib/api/object"  /* object management API */

# define OID_CATEGORY_MASK  0xc0000000
# define OID_LIGHTWEIGHT    0x80000000
# define OID_CLONE          0x40000000
# define OID_MASTER         0x00000000
# define OID_OWNER_MASK     0x3ff00000
# define OID_OWNER_OFFSET   20
# define OID_INDEX_MASK     0x000fffff
# define OID_INDEX_OFFSET   0

# define PROG_OBJNAME     0  /* object name */
# define PROG_PREVSIB     1  /* ([ int parent: int prevsib ]) */
# define PROG_NEXTSIB     2  /* ([ int parent: int nextsib ]) */
# define PROG_FIRSTCHILD  3  /* object ID of first child or clone */
