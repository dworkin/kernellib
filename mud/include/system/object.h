# define OBJECTD     "/usr/System/sys/objectd"     /* object manager */
# define OWNER_NODE  "/usr/System/obj/owner_node"  /* per-owner mgmt node */
# define API_OBJECT  "/usr/System/lib/api/object"  /* object management API */

# define OID_CATEGORY_MASK  0xc0000000
# define OID_MIDDLEWEIGHT   0x80000000
# define OID_CLONE          0xc0000000
# define OID_MASTER         0x40000000
# define OID_OWNER_MASK     0x3ff00000
# define OID_OWNER_OFFSET   20
# define OID_INDEX_MASK     0x000fffff
# define OID_INDEX_OFFSET   0

# define PDB_PATH      0  /* program name */
# define PDB_PREVIOUS  1  /* ([ int parent_oid: int previous_oid ]) */
# define PDB_NEXT      2  /* ([ int parent_oid: int next_oid ]) */
# define PDB_CHAIN     3  /* object number of first child program */

# define MWO_BUCKET_SIZE  1024
