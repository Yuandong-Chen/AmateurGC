/**
 * C语言实现的红黑树(Red Black Tree)
 *
 * @author skywang
 * @date 2013/11/18
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "rbtree.h"
#include "interface2.h"

#define rb_parent(r)    ((r)->parent)
#define rb_color(r)     ((r)->color)
#define rb_is_red(r)    (GET_COLOR(r) == RED )
#define rb_is_black(r)  (GET_COLOR(r) == BLACK)
#define rb_set_black(r) SET_COLOR(r, BLACK)
#define rb_set_red(r)   SET_COLOR(r, RED)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)   SET_COLOR(r, c)

/*
 * 创建红黑树，返回"红黑树的根"！
 */
RBRoot* create_rbtree()
{
    RBRoot *root = (RBRoot *)__malloc(sizeof(RBRoot));
    root->node = NULL;

    return root;
}

/*
 * 前序遍历"红黑树"
 */
static void preorder(RBTree tree)
{
    if(tree != NULL)
    {
        printf("%d ", GET_KEY(tree));
        preorder((RBTree)GET_LEFT(tree));
        preorder((RBTree)GET_RIGHT(tree));
    }
}
void preorder_rbtree(RBRoot *root) 
{
    if (root)
        preorder(root->node);
}

/*
 * 中序遍历"红黑树"
 */
static void inorder(RBTree tree)
{
    if(tree != NULL)
    {
        inorder((RBTree)GET_LEFT(tree));
        printf("%u ", GET_KEY(tree));
        inorder((RBTree)GET_RIGHT(tree));
    }
}

void inorder_rbtree(RBRoot *root) 
{
    if (root)
        inorder(root->node);
}

/*
 * 后序遍历"红黑树"
 */
static void postorder(RBTree tree)
{
    if(tree != NULL)
    {
        postorder((RBTree)GET_LEFT(tree));
        postorder((RBTree)GET_RIGHT(tree));
        printf("%u ", GET_KEY(tree));
    }
}

void postorder_rbtree(RBRoot *root)
{
    if (root)
        postorder(root->node);
}

/*
 * (递归实现)查找"红黑树x"中键值为key的节点
 */
static Node* search(RBTree x, Type key)
{
    if (x==NULL || GET_KEY(x)==key)
        return x;

    if (key < GET_KEY(x))
        return search((RBTree)GET_LEFT(x), key);
    else
        return search((RBTree)GET_RIGHT(x), key);
}

Node* rbtree_search(RBRoot *root, Type key)
{
    if (root)
        return search(root->node, key);

    return NULL;
}

/*
 * (非递归实现)查找"红黑树x"中键值为key的节点
 */
static Node* iterative_search(RBTree x, Type key)
{
    while ((x!=NULL) && (GET_KEY(x)!=key))
    {
        if (key < GET_KEY(x))
            x = (RBTree)GET_LEFT(x);
        else
            x = (RBTree)GET_RIGHT(x);
    }

    return x;
}
int iterative_rbtree_search(RBRoot *root, Type key)
{
    if (root)
        return iterative_search(root->node, key) ? 0 : -1;

    return -1;
}

/* 
 * 查找最小结点：返回tree为根结点的红黑树的最小结点。
 */
static Node* minimum(RBTree tree)
{
    if (tree == NULL)
        return NULL;

    while(GET_LEFT(tree) != 0) /* (void *)0 === NULL */
        tree = (RBTree)GET_LEFT(tree);
    return tree;
}

int rbtree_minimum(RBRoot *root, unsigned int *val)
{
    Node *node;

    if (root)
        node = minimum(root->node);

    if (node == NULL)
        return -1;

    *val = GET_KEY(node);
    return 0;
}
 
/* 
 * 查找最大结点：返回tree为根结点的红黑树的最大结点。
 */
static Node* maximum(RBTree tree)
{
    if (tree == NULL)
        return NULL;

    while(GET_RIGHT(tree) != 0)
        tree = (RBTree)GET_RIGHT(tree);
    return tree;
}

int rbtree_maximum(RBRoot *root, unsigned int *val)
{
    Node *node;

    if (root)
        node = maximum(root->node);

    if (node == NULL)
        return -1;

    *val = GET_KEY(node);
    return 0;
}

/* 
 * 找结点(x)的后继结点。即，查找"红黑树中数据值大于该结点"的"最小结点"。
 */
static Node* rbtree_successor(RBTree x)
{
    // 如果x存在右孩子，则"x的后继结点"为 "以其右孩子为根的子树的最小结点"。
    if (GET_RIGHT(x) != 0)
        return minimum((RBTree)GET_RIGHT(x));

    // 如果x没有右孩子。则x有以下两种可能：
    // (01) x是"一个左孩子"，则"x的后继结点"为 "它的父结点"。
    // (02) x是"一个右孩子"，则查找"x的最低的父结点，并且该父结点要具有左孩子"，找到的这个"最低的父结点"就是"x的后继结点"。
    Node* y = x->parent;
    while ((y!=NULL) && (x == (RBTree)GET_RIGHT(y)))
    {
        x = y;
        y = y->parent;
    }

    return y;
}
 
/* 
 * 找结点(x)的前驱结点。即，查找"红黑树中数据值小于该结点"的"最大结点"。
 */
static Node* rbtree_predecessor(RBTree x)
{
    // 如果x存在左孩子，则"x的前驱结点"为 "以其左孩子为根的子树的最大结点"。
    if (GET_LEFT(x) != 0)
        return maximum((RBTree)GET_LEFT(x));

    // 如果x没有左孩子。则x有以下两种可能：
    // (01) x是"一个右孩子"，则"x的前驱结点"为 "它的父结点"。
    // (01) x是"一个左孩子"，则查找"x的最低的父结点，并且该父结点要具有右孩子"，找到的这个"最低的父结点"就是"x的前驱结点"。
    Node* y = x->parent;
    while ((y!=NULL) && (x==(RBTree)GET_LEFT(y)))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

/* 
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y                
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry     
 *     /   \                       /  \
 *    ly   ry                     lx  ly  
 *
 *
 */
static void rbtree_left_rotate(RBRoot *root, Node *x)
{
    // 设置x的右孩子为y
    Node *y = (Node *)GET_RIGHT(x);

    // 将 “y的左孩子” 设为 “x的右孩子”；
    // 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
    SET_RIGHT(x, GET_LEFT(y));
    if (GET_LEFT(y) != 0)
        SET_PARENT(GET_LEFT(y),(unsigned int)x);

    // 将 “x的父亲” 设为 “y的父亲”
    y->parent = x->parent;

    if (x->parent == NULL)
    {
        //tree = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
        root->node = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
    }
    else
    {
        if (GET_LEFT(x->parent) == (unsigned int)x)
            SET_LEFT(x->parent,(unsigned int)y);    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
        else
            SET_RIGHT(x->parent,(unsigned int)y);     // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
    }
    
    // 将 “x” 设为 “y的左孩子”
    SET_LEFT(y,(unsigned int)x);
    // 将 “x的父节点” 设为 “y”
    x->parent = y;
}

/* 
 * 对红黑树的节点(y)进行右旋转
 *
 * 右旋示意图(对节点y进行左旋)：
 *            py                               py
 *           /                                /
 *          y                                x                  
 *         /  \      --(右旋)-->            /  \                     #
 *        x   ry                           lx   y  
 *       / \                                   / \                   #
 *      lx  rx                                rx  ry
 * 
 */
static void rbtree_right_rotate(RBRoot *root, Node *y)
{
    // 设置x是当前节点的左孩子。
    Node *x = (Node *)GET_LEFT(y);

    // 将 “x的右孩子” 设为 “y的左孩子”；
    // 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
    SET_LEFT(y,GET_RIGHT(x));
    if (GET_RIGHT(x) != 0)
        SET_PARENT(GET_RIGHT(x),(unsigned int)y);

    // 将 “y的父亲” 设为 “x的父亲”
    x->parent = y->parent;

    if (y->parent == NULL) 
    {
        //tree = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
        root->node = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
    }
    else
    {
        if (y == (Node *)GET_RIGHT(y->parent))
            SET_RIGHT(y->parent,(unsigned int)x);    // 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
        else
            SET_LEFT(y->parent,(unsigned int)x);    // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
    }

    // 将 “y” 设为 “x的右孩子”
    SET_RIGHT(x,(unsigned int)y);

    // 将 “y的父节点” 设为 “x”
    y->parent = x;
}

/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入节点之后(失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
static void rbtree_insert_fixup(RBRoot *root, Node *node)
{
    Node *parent, *gparent;

    // 若“父节点存在，并且父节点的颜色是红色”
    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        //若“父节点”是“祖父节点的左孩子”
        if (parent == (RBTree)GET_LEFT(gparent))
        {
            // Case 1条件：叔叔节点是红色
            {
                Node *uncle = (RBTree)GET_RIGHT(gparent);
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            // Case 2条件：叔叔是黑色，且当前节点是右孩子
            if (GET_RIGHT(parent) == (unsigned int)node)
            {
                Node *tmp;
                rbtree_left_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            // Case 3条件：叔叔是黑色，且当前节点是左孩子。
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_right_rotate(root, gparent);
        } 
        else//若“z的父节点”是“z的祖父节点的右孩子”
        {
            // Case 1条件：叔叔节点是红色
            {
                Node *uncle = (RBTree)GET_LEFT(gparent);
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            // Case 2条件：叔叔是黑色，且当前节点是左孩子
            if (GET_LEFT(parent) == (unsigned int)node)
            {
                Node *tmp;
                rbtree_right_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            // Case 3条件：叔叔是黑色，且当前节点是右孩子。
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_left_rotate(root, gparent);
        }
    }

    // 将根节点设为黑色
    rb_set_black(root->node);
}

/*
 * 添加节点：将节点(node)插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
static void rbtree_insert(RBRoot *root, Node *node)
{
    Node *y = NULL;
    Node *x = root->node;

    // 1. 将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
    while (x != NULL)
    {
        y = x;
        if (GET_KEY(node) < GET_KEY(x))
            x = (RBTree)GET_LEFT(x);
        else
            x = (RBTree)GET_RIGHT(x);
    }
    rb_parent(node) = y;

    if (y != NULL)
    {
        if (GET_KEY(node) < GET_KEY(y))
            SET_LEFT(y,(unsigned int)node);                // 情况2：若“node所包含的值” < “y所包含的值”，则将node设为“y的左孩子”
        else
            SET_RIGHT(y,(unsigned int)node);            // 情况3：(“node所包含的值” >= “y所包含的值”)将node设为“y的右孩子” 
    }
    else
    {
        root->node = node;                // 情况1：若y是空节点，则将node设为根
    }

    // 2. 设置节点的颜色为红色
    rb_set_color(node, RED);

    // 3. 将它重新修正为一颗二叉查找树
    rbtree_insert_fixup(root, node);
}

/*
 * 创建结点
 *
 * 参数说明：
 *     key 是键值。
 *     parent 是父结点。
 *     left 是左孩子。
 *     right 是右孩子。
 */
static Node* create_rbtree_node(Node *parent, Node *left, Node* right, size_t size)
{
    Node* p;

    if ((p = (Node *)__malloc(sizeof(Node) + size)) == NULL)
        return NULL;

    SET_LEFT(p,(unsigned int)left);
    SET_RIGHT(p,(unsigned int)right);
    p->parent = parent;
    rb_set_black(p);

    return p;
}

/* 
 * 新建结点(节点键值为key)，并将其插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根
 *     key 插入结点的键值
 * 返回值：
 *     0，插入成功
 *     -1，插入失败
 */
int insert_rbtree(RBRoot *root, Type key)
{
    Node *node;    // 新建结点

    // 不允许插入相同键值的节点。
    // (若想允许插入相同键值的节点，注释掉下面两句话即可！)
    if (search(root->node, key) != NULL)
        return -1;

    // 如果新建结点失败，则返回。
    if ((node=create_rbtree_node(NULL, NULL, NULL, 0)) == NULL)
        return -1;

    rbtree_insert(root, node);

    return 0;
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
 */
static void rbtree_delete_fixup(RBRoot *root, Node *node, Node *parent)
{
    Node *other;

    while ((!node || rb_is_black(node)) && node != root->node)
    {
        if (GET_LEFT(parent) == (unsigned int)node)
        {
            other = (RBTree)GET_RIGHT(parent);
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的  
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_left_rotate(root, parent);
                other = (RBTree)GET_RIGHT(parent);
            }
            if ((!GET_LEFT(other) || rb_is_black(GET_LEFT(other))) &&
                (!GET_RIGHT(other) || rb_is_black(GET_RIGHT(other))))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的  
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!GET_RIGHT(other) || rb_is_black(GET_RIGHT(other)))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。  
                    rb_set_black(GET_LEFT(other));
                    rb_set_red(other);
                    rbtree_right_rotate(root, other);
                    other = (RBTree)GET_RIGHT(parent);
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, GET_COLOR(parent));
                rb_set_black(parent);
                rb_set_black(GET_RIGHT(other));
                rbtree_left_rotate(root, parent);
                node = root->node;
                break;
            }
        }
        else
        {
            other = (RBTree)GET_LEFT(parent);
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的  
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_right_rotate(root, parent);
                other = (RBTree)GET_LEFT(parent);
            }
            if ((!GET_LEFT(other) || rb_is_black(GET_LEFT(other))) &&
                (!GET_RIGHT(other) || rb_is_black(GET_RIGHT(other))))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的  
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!GET_LEFT(other) || rb_is_black(GET_LEFT(other)))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。  
                    rb_set_black(GET_RIGHT(other));
                    rb_set_red(other);
                    rbtree_left_rotate(root, other);
                    other = (RBTree)GET_LEFT(parent);
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, GET_COLOR(parent));
                rb_set_black(parent);
                rb_set_black(GET_LEFT(other));
                rbtree_right_rotate(root, parent);
                node = root->node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

/* 
 * 删除结点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     node 删除的结点
 */
void rbtree_delete(RBRoot *root, Node *node)
{
    Node *child, *parent;
    int color;

    // 被删除节点的"左右孩子都不为空"的情况。
    if ( (GET_LEFT(node) != 0) && (GET_RIGHT(node) != 0) ) 
    {
        // 被删节点的后继节点。(称为"取代节点")
        // 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
        Node *replace = node;

        // 获取后继节点
        replace = (RBTree)GET_RIGHT(replace);
        while (GET_LEFT(replace) != 0)
            replace = (RBTree)GET_LEFT(replace);

        // "node节点"不是根节点(只有根节点不存在父节点)
        if (rb_parent(node))
        {
            if (GET_LEFT(rb_parent(node))== (unsigned int)node)
                SET_LEFT(rb_parent(node), (unsigned int)replace);
            else
                SET_RIGHT(rb_parent(node),(unsigned int)replace);
        } 
        else 
            // "node节点"是根节点，更新根节点。
            root->node = replace;

        // child是"取代节点"的右孩子，也是需要"调整的节点"。
        // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
        child = (RBTree)GET_RIGHT(replace);
        parent = rb_parent(replace);
        // 保存"取代节点"的颜色
        color = GET_COLOR(replace);

        // "被删除节点"是"它的后继节点的父节点"
        if (parent == node)
        {
            parent = replace;
        } 
        else
        {
            // child不为空
            if (child)
                rb_set_parent(child, parent);

            SET_LEFT(parent,(unsigned int)child);
            SET_RIGHT(replace,GET_RIGHT(node));
            rb_set_parent((RBTree)GET_RIGHT(node), replace);
        }

        replace->parent = node->parent;
        rb_set_color(replace,GET_COLOR(node));
        SET_LEFT(replace,GET_LEFT(node));
        SET_PARENT(GET_LEFT(node),(unsigned int)replace);

        if (color == BLACK)
            rbtree_delete_fixup(root, child, parent);
        __free(node);

        return ;
    }

    if (GET_LEFT(node) !=0)
        child = (RBTree)GET_LEFT(node);
    else 
        child = (RBTree)GET_RIGHT(node);

    parent = node->parent;
    // 保存"取代节点"的颜色
    color = GET_COLOR(node);

    if (child)
        child->parent = parent;

    // "node节点"不是根节点
    if (parent)
    {
        if (GET_LEFT(parent) == (unsigned int)node)
            SET_LEFT(parent, (unsigned int)child);
        else
            SET_RIGHT(parent, (unsigned int)child);
    }
    else
        root->node = child;

    if (color == BLACK)
        rbtree_delete_fixup(root, child, parent);
    __free(node);
}

/* 
 * 删除键值为key的结点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     key 键值
 */
void delete_rbtree(RBRoot *root, Type key)
{
    Node *z, *node; 

    if ((z = search(root->node, key)) != NULL){
        rbtree_delete(root, z);
    }
        
}

/*
 * 销毁红黑树
 */
static void rbtree_destroy(RBTree tree)
{
    if (tree==NULL)
        return ;

    if (GET_LEFT(tree) != 0)
        rbtree_destroy((RBTree)GET_LEFT(tree));
    if (GET_RIGHT(tree) != 0)
        rbtree_destroy((RBTree)GET_RIGHT(tree));

    __free(tree);
}

void destroy_rbtree(RBRoot *root)
{
    if (root != NULL)
        rbtree_destroy(root->node);

    __free(root);
}

/*
 * 打印"红黑树"
 *
 * tree       -- 红黑树的节点
 * key        -- 节点的键值 
 * direction  --  0，表示该节点是根节点;
 *               -1，表示该节点是它的父结点的左孩子;
 *                1，表示该节点是它的父结点的右孩子。
 */
static void rbtree_print(RBTree tree, Type key, int direction)
{
    if(tree != NULL)
    {
        if(direction==0)    // tree是根节点
            printf("0x%x(B) is root\n", GET_KEY(tree));
        else                // tree是分支节点
            printf("0x%x(%s) is 0x%x's %6s child\n", GET_KEY(tree), rb_is_red(tree)?"R":"B", key, direction==1?"right" : "left");

        rbtree_print((RBTree)GET_LEFT(tree), GET_KEY(tree), -1);
        rbtree_print((RBTree)GET_RIGHT(tree), GET_KEY(tree),  1);
    }
}

void print_rbtree(RBRoot *root)
{
    if (root!=NULL && root->node!=NULL)
        rbtree_print(root->node, GET_KEY(root->node), 0);
}

/****************************wrap up functions above**********************************/

extern RBRoot *close_rbtree_root;
extern volatile unsigned int volume;

int __rb_init()
{
    int result;
    result = __init();
    close_rbtree_root = create_rbtree();
    return result; 
}

void __rb_exit()
{
    if(close_rbtree_root != NULL)
        destroy_rbtree(close_rbtree_root);

    close_rbtree_root = NULL;
    __exit();
}

void *__rb_calloc(size_t size)
{
    Node *node;    // 新建结点
    unsigned int rsize;
    // 如果新建结点失败，则返回。
    if ((node=create_rbtree_node(NULL, NULL, NULL, size)) == NULL)
        return NULL;

    rsize = GET_SIZE(node);
    memset(((char *)node + WSIZE),0,rsize - 2*DSIZE - WSIZE);
    rbtree_insert(close_rbtree_root, node);

    return (void *)((unsigned int)node + WSIZE);
}

void __rb_free(void *bp)
{
    delete_rbtree(close_rbtree_root, GET_KEY(((unsigned int)bp - WSIZE)));
}

RBTree __rb_search(RBTree x, Type key)
{
    if (x==NULL || ((GET_KEY(x) + 2*DSIZE <= key) && (GET_KEY(x) + GET_SIZE(x) - WSIZE > key )))
        return x;

    if (key < GET_KEY(x))
        return __rb_search((RBTree)GET_LEFT(x), key);
    else
        return __rb_search((RBTree)GET_RIGHT(x), key);

    return NULL;
}

RBTree __rb_find_node(Type key)
{
    unsigned int i,j;
    
    if( (rbtree_minimum(close_rbtree_root,&i) == 0) 
        && (rbtree_maximum(close_rbtree_root,&j) == 0)){
        if((key < i + 2*DSIZE) || (key > j + GET_SIZE_AT_ADDR(j) - WSIZE)){
            return NULL;
        }
        else{
            return __rb_search(close_rbtree_root->node, key);
        }
    }

    return NULL;
}

void print_info()
{
    print_rbtree(close_rbtree_root);
}
