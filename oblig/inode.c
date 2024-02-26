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
// same as return num_inode_ids++;
static int next_inode_id( )
{
    int retval = num_inode_ids;
    num_inode_ids += 1;
    return retval;
}

/*
 * Helper function that adds given child to parent->children by allocating space
 */
int add_inode_to_parent( struct inode* parent, struct inode* child) {
    struct inode **new_children = realloc(parent->children, (parent->num_children+1) * sizeof(struct inode*));
    if (new_children == NULL) {
        perror("Allocating children in 'create_dir/file' failed");
        free(child->name);
        free(child);
        num_inode_ids--;
        return -1;
    }
    parent->children = new_children;
    parent->children[parent->num_children++] = child;
    return 0;
}

struct inode* create_file( struct inode* parent, char* name, int size_in_bytes )
{
    // if name is not unique or parent doesnt exist, return NULL
    if (parent == NULL || find_inode_by_name(parent, name)) {
        return NULL;
    }

    // allocates space for new directory
    struct inode *new_child = malloc(sizeof(struct inode));
    if (new_child == NULL) {
        perror("Allocating new child in 'create_file' failed");
        return NULL;
    }
    // initializing new_child values+
    new_child->id = next_inode_id();
    new_child->name = strdup(name); // allocates and assigns using strdup
    new_child->is_directory = 0;
    new_child->num_children = 0;
    new_child->children = NULL; // (?)
    new_child->filesize = size_in_bytes;

    // handles block allocation
    int blocks = blocks_needed(size_in_bytes);
    new_child->num_blocks = blocks;
    new_child->blocks = malloc(sizeof(size_t) * blocks);
    if (new_child->blocks == NULL) {
        perror("Allocating blocks for %s in 'create_file' failed");
        free(new_child->name);
        free(new_child);
        num_inode_ids--;
        return NULL;
    }
    // allocates blocks: if -1 is returned, allocation fails and NULL is returned
    for (int i = 0; i < blocks; ++i) {
        int result = allocate_block();
        if (result == -1) {
            for (int j = 0; j < i; ++j) {
                free_block((int) new_child->blocks[j]);
            }
            free(new_child->blocks);
            free(new_child->name);
            free(new_child);
            num_inode_ids--;
            return NULL;
        }
        new_child->blocks[i] = result;
    }

    // adds inode to parent, response -1 is fail, 0 is success
    int response = add_inode_to_parent(parent, new_child);
    if (response != 0) {
        return NULL;
    }

    return new_child;
}

struct inode* create_dir( struct inode* parent, char* name )
{
    // parent exist and name is not unique
    if (parent && find_inode_by_name(parent, name)) {
        return NULL;
    }

    // allocates space for new directory
    struct inode *new_child = malloc(sizeof(struct inode));
    if (new_child == NULL) {
        perror("Allocating new child in 'create_dir' failed");
        return NULL;
    }
    // initializing new_child values
    new_child->id = next_inode_id();
    new_child->name = strdup(name); // allocates and assigns using strdup
    new_child->is_directory = 1;
    new_child->num_children = 0;
    new_child->children = NULL; // (?)
    new_child->filesize = 0 ;
    new_child->num_blocks = 0;
    new_child->blocks = NULL;

    // case: parent is null, meaning the new root
    if (parent == NULL) {
        return new_child;
    }

    // adds inode to parent, response -1 is fail, 0 is success
    int response = add_inode_to_parent(parent,  new_child);
    if (response != 0) {
        return NULL;
    }

    return new_child;
}

struct inode* find_inode_by_name( struct inode* parent, char* name )
{
    // proceeds to search if parent exists
    if (parent != NULL) {

        // case: parent-name is "name"
        if (strcmp(parent->name, name) == 0) {
            return parent;
        }
        // case: parent is dir -> look through children for name
        if (parent->is_directory) {
            if (parent->num_children > 0) {
                for (int i = 0; i < parent->num_children; ++i) {
                    struct inode *child = parent->children[i];
                    if (strcmp(child->name, name) == 0) {
                        return child;
                    }
                }
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
    num_inode_ids++; // increases global id-count
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