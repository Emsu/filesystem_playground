#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

static uint32_t const AUFS_MAGIC_NUMBER = 0x13131313;

static void aufs_put_super(struct super_block *sb)
{
    pr_debug("aufs super block destroyed\n");
}

static struct super_operations const aufs_super_ops = {
    .put_super = aufs_put_super,
};

static int aufs_fill_sb(struct super_block *sb, void *data, int silent)
{
    struct inode *root = NULL;
    
    sb->s_magic = AUFS_MAGIC_NUMBER;
    sb->s_op = &aufs_super_ops;

    root = new_inode(sb);
    if (!root)
    {
        pr_err("inode allocation failed\n");
        return -ENOMEM;
    }

    root->i_ino = 0;
    root->i_sb = sb;
    root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
    inode_init_owner(root, NULL, S_IFDIR);

    sb->s_root = d_make_root(root);
    if (!sb->s_root)
    {
        pr_err("root creation failed\n");
        return -ENOMEM;
    }

    return 0;
};

/* Called during filesystem assembly*/
static struct dentry *aufs_mount(
        struct file_system_type *type,
        int flags,
        char const *dev,
        void *data)
{
    // mount_bdev performs most of the work in making this filesystem mountable.
    // By proxy, aufs_fill_sb is the most important function we are writing for mounting.
    struct dentry *const entry = mount_bdev(type, flags, dev, data, aufs_fill_sb);

    if (IS_ERR(entry))
        pr_err("aufs mounting failed\n");
    else
        pr_debug("auf mounted\n");
    return entry;
};

static struct file_system_type aufs_type = {
    .owner = THIS_MODULE,
    .name = "aufs",
    .mount = aufs_mount,
    .kill_sb = kill_block_super,
    .fs_flags = FS_REQUIRES_DEV,
};

static int __init aufs_init(void)
{
    /*
    int ret = aufs_inode_cache_create();
    if (ret != 0){
        pr_err("cannot create inode cache\n");
        return ret;
    }

    ret = register_filesystem(&aufs_type);
    if (ret != 0) {
        aufs_inode_cache_destroy();
        pr_err("cannot register_filesystem\n");
        return ret;
    }
    */
    register_filesystem(&aufs_type);

    pr_debug("aufs module loaded\n");
    return 0;
}; 

static void __exit aufs_fini(void)
{
    unregister_filesystem(&aufs_type);
    pr_debug("aufs module unloaded\n");
};

module_init(aufs_init);
module_exit(aufs_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("emsu");
