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
    /* to be implemented */
    return 0;
}

int is_node_in_parent( struct inode* parent, struct inode* node )
{
    /* to be implemented */
    return 0;
}

int delete_file( struct inode* parent, struct inode* node )
{
    // node er fil som skal slettes
    // parent er mappe som skal leses gjennom

    // hvis parent inneholder node:  inode kan bli slettet, return 0
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

void save_inodes( char* master_file_table, struct inode* root )
{
    /* to be implemented */
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
//    FILE *file = fopen(master_file_table, "rb");
//
//    struct inode *root = malloc(sizeof(struct inode));
//    fread(&root->id, sizeof(int), 1, file);
//
//    // handles name
//    int name_length; fread(&name_length, sizeof(int), 1, file);
//    root->name = malloc(name_length);
//    fread(root->name, sizeof(char), name_length, file);
//    root->name[name_length] = '\0';
//
//    fread(&root->is_directory, sizeof(char), 1, file);
//    fread(&root->num_children, sizeof(int), 1, file);
//
//    printf("ID: %d\nname_len: %d\nName: %s\nis_dir: %d\nnum_children: %d\n",
//           root->id, name_length, root->name, root->is_directory, root->num_children);
//
//    free(root);

    // with error-check (longer)
    FILE *file;
    int id;
    int name_length;
    char is_dir;
    int num_children;

    // allocates space for inode //TODO: free?
    struct inode *root = malloc(sizeof(struct inode));

    // attempts to open file using fopen with read-only and "binary-mode"
    if ((file = fopen(master_file_table, "rb")) == NULL) {
        perror("Error opening master file table");
        exit(-1);
    }

    // reads id and name-length from file
    size_t id_rc = fread(&id, sizeof(int), 1, file);
    size_t len_rc = fread(&name_length, sizeof(int), 1, file);
    if (id_rc != 1 || len_rc != 1) {
        perror("Error reading id or name length");
        fclose(file);
        return NULL;
    } root->id = id;

    // reads and saves the name
    char name[name_length]; //TODO: use malloc?
    size_t name_rc = fread(&name, sizeof(char), name_length, file);
    if (name_rc < name_length){
        perror("Error reading name");
        fclose(file);
        return NULL;
    }
    name[name_length] = '\0'; // sets null-byte
    root->name = name;

    // reads flag as a character 1 = directory 0 = file
    if (fread(&is_dir, sizeof(char), 1, file) != 1){
        perror("Error reading flag");
        fclose(file);
        return NULL;
    } root->is_directory = is_dir;

    // reads number of children
    if (!fread(&num_children, sizeof(int), 1, file)) {
        perror("Error reading num children");
        fclose(file);
        return NULL;
    } root->num_children = num_children;

    // is_dir == 1 and num_children > 0
    if (is_dir && num_children) {
        // scan children and assign to root->children
        // recursively?
        //TODO: find structure of next bytes
    }
    else { root->children = NULL; }

    printf("ID: %d\nName length: %d\nName: %s\nFlag: %d\nNum children: %d\n", id, name_length, name, flag, num_children);

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

