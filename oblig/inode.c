#include "allocation.h"
#include "inode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* The number of bytes in a block.
 * Do not change.
 */
#define BLOCKSIZE 4096

/* The lowest unused node ID.
 * Do not change.
 */
static int num_inode_ids = 0;

/* This helper function computes the number of blocks that you must allocate
 * on the simulated disk for a give file system in bytes. You don't have to use
 * it.
 * Do not change.
 */
static int blocks_needed( int bytes )
{
    int blocks = bytes / BLOCKSIZE;
    if( bytes % BLOCKSIZE != 0 )
        blocks += 1;
    return blocks;
}

/* This helper function returns a new integer value when you create a new inode.
 * This helps you do avoid inode reuse before 2^32 inodes have been created. It
 * keeps the lowest unused inode ID in the global variable num_inode_ids.
 * Make sure to update num_inode_ids when you have loaded a simulated disk.
 * Do not change.
 */
static int next_inode_id( )
{
    int retval = num_inode_ids;
    num_inode_ids += 1;
    return retval;
}

struct inode* create_file( struct inode* parent, char* name, int size_in_bytes )
{
    // gå gjennom hele "parent-mappen"
    // hvis navnet finnes, return NULL
    // hvis ikke: size_in_bytes er antall bytes som skal lagres på disken
    // må brue allocate_block

    // hvis disk ikke har nok plass for allocate_block, "relase all resources" og return NULL

    /* to be implemented */
    return NULL;
}

struct inode* create_dir( struct inode* parent, char* name )
{

    /* to be implemented */
    return NULL;
}

struct inode* find_inode_by_name( struct inode* parent, char* name )
{
    printf("-> From: %d looking for %s\n", parent->id, name);
    // iterer gjennom parent og finn inode med "name"
    // success: return peker til inode
    // fails: return NULL

    // case: parent-name is "name"
    if (strcmp(parent->name, name) == 0) return parent;

    // case: look through children-nodes
    else if (parent->num_children > 0) {
        for (int i = 0; i < parent->num_children; ++i) {
            struct inode *child = find_inode_by_name(parent->children[i], name);
            if (child != NULL){
                printf("Found %s, id: %d\n", child->name, child->id);
                return child;
            }
        }
    }

    return NULL;
}

static int verified_delete_in_parent( struct inode* parent, struct inode* node )
{
    // hjelpemetode (ikke nødvendig)
    /* to be implemented */
    return 0;
}

int is_node_in_parent( struct inode* parent, struct inode* node )
{
    // hjelpemetode (ikke nødvendig)
    /* to be implemented */
    return 0;
}

int delete_file( struct inode* parent, struct inode* node )
{
    // node er fil som skal slettes
    // parent er mappe som skal leses gjennom
    // (må sjekke om node er i parent og node er fil / skal være tom)

    // hvis: parent inneholder node:  inode kan bli slettet, return 0
    // ellers: return -1

    // må bruke "free_block" fra allocation for å frigjøre minne for return
    /* to be implemented */
    return 0;
}

int delete_dir( struct inode* parent, struct inode* node )
{
    // parent er "parent-mappe"
    // node er mappen som skal slettes

    // kan slettes om:
    // parent er direkte parent, altså rett over
    // node er tom og ikke inneholder filer

    // success: return 0
    // fail: return -1;

    /* to be implemented */
    return 0;
}

/* The function save_inode is a recursive functions that is
 * called by save_inodes to store a single inode on disk,
 * and call itself recursively for every child if the node
 * itself is a directory.
 */
static void save_inode( FILE* file, struct inode* node )
{
    if( !node ) return;

    int len = strlen( node->name ) + 1;

    fwrite( &node->id, 1, sizeof(int), file );
    fwrite( &len, 1, sizeof(int), file );
    fwrite( node->name, 1, len, file );
    fwrite( &node->is_directory, 1, sizeof(char), file );
    if( node->is_directory )
    {
        fwrite( &node->num_children, 1, sizeof(int), file );
        for( int i=0; i<node->num_children; i++ )
        {
            struct inode* child = node->children[i];
            size_t id = child->id;
            fwrite( &id, 1, sizeof(size_t), file );
        }

        for( int i=0; i<node->num_children; i++ )
        {
            struct inode* child = node->children[i];
            save_inode( file, child );
        }
    }
    else
    {
        fwrite( &node->filesize, 1, sizeof(int), file );
        fwrite( &node->num_blocks, 1, sizeof(int), file );
        for( int i=0; i<node->num_blocks; i++ )
        {
            fwrite( &node->blocks[i], 1, sizeof(size_t), file );
        }
    }
}

void save_inodes( char* master_file_table, struct inode* root )
{
    if( root == NULL )
    {
        fprintf( stderr, "root inode is NULL\n" );
        return;
    }

    FILE* file = fopen( master_file_table, "w" );
    if( !file )
    {
        fprintf( stderr, "Failed to open file %s\n", master_file_table );
        return;
    }

    save_inode( file, root );

    fclose( file );
}

struct inode* create_inode(FILE *file) {

    // reads file until end
    struct inode *node = (struct inode*) malloc(sizeof(struct inode));

    // TODO: error checks?

    // info every inode contains
    // id, name_length, name, flag
    fread(&node->id, sizeof(int), 1, file);
    int name_length;
    fread(&name_length, sizeof(int), 1, file);
    node->name = malloc(name_length);
    fread(node->name, sizeof(char), name_length, file);
    fread(&node->is_directory, sizeof(char), 1, file);

    // inode represents a directory
    if (node->is_directory) {

        // reads children-count
        fread(&node->num_children, sizeof(int), 1, file);

        // If it is 0, children is NULL.
        // else children is array of pointers to child inodes
        if (node->num_children > 0) {

            // allocates memory for child-node
            node->children = malloc(node->num_children * sizeof(struct inode *));

            // reads child IDs to list
            int *children = malloc(node->num_children);
            for (int i = 0; i < node->num_children; ++i) {
                fread(&children[i], sizeof(size_t), 1, file);
            }

            // gets the actual child-nodes (the next inode bytes-section)
            for (int i = 0; i < node->num_children; ++i) {
                node->children[i] = create_inode(file);
            }
            free(children);
        } else node->children = NULL;

    }
    // Inode represents a file
    else {
        fread(&node->filesize, sizeof(int), 1, file);
        fread(&node->num_blocks, sizeof(int), 1, file);

        // allocates memory for block
        node->blocks = malloc(node->num_blocks * sizeof(size_t));
        for (int i = 0; i < node->num_blocks; ++i) {
            fread(&node->blocks[i], sizeof(size_t), 1, file);
        }
    }
    return node;
}

/*
 * Creates inode in memory from master file table for each corresponding entry in file.
 * If loading succeeds, returned inode
 */
struct inode* load_inodes( char* master_file_table )
{
    FILE *file = fopen(master_file_table, "rb");
    if(!file){
        fprintf( stderr, "Failed to open file %s\n", master_file_table );
        return NULL;
    }

    struct inode *root = create_inode(file);
    debug_fs(root);

    fclose(file);
    return root;
}

/* This static variable is used to change the indentation while debug_fs
 * is walking through the tree of inodes and prints information.
 */
static int indent = 0;

/* Do not change. */
void debug_fs( struct inode* node )
{
    if( node == NULL ) return;
    for( int i=0; i<indent; i++ )
        printf("  ");

    if( node->is_directory )
    {
        printf("%s (id %d)\n", node->name, node->id );
        indent++;
        for( int i=0; i<node->num_children; i++ )
        {
            struct inode* child = (struct inode*)node->children[i];
            debug_fs( child );
        }
        indent--;
    }
    else
    {
        printf("%s (id %d size %db blocks ", node->name, node->id, node->filesize );
        for( int i=0; i<node->num_blocks; i++ )
        {
            printf("%d ", (int)node->blocks[i]);
        }
        printf(")\n");
    }
}

/* Do not change. */
void fs_shutdown( struct inode* inode )
{
    if( !inode ) return;

    if( inode->is_directory )
    {
        for( int i=0; i<inode->num_children; i++ )
        {
            fs_shutdown( inode->children[i] );
        }
    }

    if( inode->name )     free( inode->name );
    if( inode->children ) free( inode->children );
    if( inode->blocks )   free( inode->blocks );
    free( inode );
}


//TODO:  remove
/*
    Representation of a master_file_table (from xxd -g 1 master_file_table)
    each pair is a byte distributed over the structure of an inode

    ---------------------------------------------------------------------------------------
        id			name_len	name   flag  children num
    00 00 00 00 | 02 00 00 00 | 2f 00 | 01 | 02 00 00 00 |

    = 0 | 2 | / | 1 | 2

                    children pointers
    01 00 00 00 00 00 00 00 | 02 00 00 00 00 00 00 00
    ---------------------------------------------------------------------------------------
        id			name_len		name			  flag	 filesize	  num blocks
    01 00 00 00 | 07 00 00 00 | 6b 65 72 6e 65 6c 00 | 00 | 20 4e 00 00 | 05 00 00 00 |

    = 1 | 7 | kernel | 0 | 20000 | 5

                                    blocks
    00 00 00 00 00 00 00 00 | 01 00 00 00 00 00 00 00 | 02 00 00 00 00 00 00 00 |
    03 00 00 00 00 00 00 00 | 04 00 00 00 00 00 00 00 |
    ---------------------------------------------------------------------------------------
        id			name_len		name	 flag	children num
    02 00 00 00 | 04 00 00 00 | 65 74 63 00 | 01 | 01 00 00 00 |

    = 2 | 4 | ect | 1 | 1

                    children pointer
                03 00 00 00 00 00 00 00
    ---------------------------------------------------------------------------------------
        id			name_len		name		   flag		filesize	 num blocks
    03 00 00 00 | 06 00 00 00 | 68 6f 73 74 73 00 | 00 | d0 07 00 00 | 01 00 00 00 |

    = 3 | 6 | hosts | 0 | 2000 | 1

            block
    05 00 00 00 00 00 00 00
    ---------------------------------------------------------------------------------------
 */