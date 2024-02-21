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
    // iterer gjennom parent og finn inode med "name"
    // success: return peker til inode
    // fails: return NULL

    /* to be implemented */
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

/*
 * Creates inode in memory from master file table for each corresponding entry in file.
 * If loading succeeds, returned inode
 */
struct inode* load_inodes( char* master_file_table )
{
    printf("\n--------------------------------------------------------\n\n");
    printf("Hello welcome to loan_inodes\n\n");

    // without error-check (shorter)
    FILE *file = fopen(master_file_table, "rb");

    struct inode *root = malloc(sizeof(struct inode));

    fread(&root->id, sizeof(int), 1, file);
    int name_length; fread(&name_length, sizeof(int), 1, file);
    root->name = malloc(name_length);
    fread(root->name, sizeof(char), name_length, file);
    fread(&root->is_directory, sizeof(char), 1, file);

    // is_dir == 1 and num_children > 0
    if (root->is_directory) {

        // reads how many children
        fread(&root->num_children, sizeof(int), 1, file);

        printf("ID: %d\nname_len: %d\nName: %s\nis_dir: %d\nnum_children: %d\n",root->id, name_length, root->name, root->is_directory, root->num_children);

        for (int i = 0; i < 1; ++i) {
            // TODO: recursive read
            int id2, nm2;
            fread(&id2, sizeof(int), 1, file);
            fread(&nm2, sizeof(int), 1, file);
            printf("%d\n%d\n", id2, nm2);
//            char *name2 = malloc(nm2);
//            fread(&name2, sizeof(char), name_length, 1);
//            printf("%d %d %s\n", id2, nm2, name2);
        }
        // scan children and assign to root->children
        // recursively?
        //TODO: find structure of next bytes
        // -> 8 byte hvert barn (id)

        // flag=1 -> id | lengde | navn | flag | num_children | children_id |
        // flag=0 -> id | lengde | navn | flag | num_children | filesize | num_blocks | blocks |
    }
    else {
        root->children = NULL;
        // contains filsize, num_blocks and blocks
    }
//
//    printf("ID: %d\nName length: %d\nName: %s\nFlag: %d\nNum children: %d\n",
//           id, name_length, name, is_dir, num_children);

    // frees file-memory
    fclose(file);

    // les hele disk og "master_file_table" og lag inode for hver eneste "entry" i filen
    // lag inode for hver mappe og fil

    // success: inode returned skal være root, navn-field skal peke tl streng "/"
    // fail: return NULL ?

    printf("\n--------------------------------------------------------\n\n");
    /* to be implemented */
    return NULL;
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

