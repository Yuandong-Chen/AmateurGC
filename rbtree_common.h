#ifndef __COMMON_H_
#define __COMMON_H_

typedef unsigned int Type;

// 红黑树的节点
typedef struct RBTreeNode{
    // unsigned char color;        // 颜色(RED 或 BLACK)
    // Type   key;                    // 关键字(键值)
    // struct RBTreeNode *left;    // 左孩子
    // struct RBTreeNode *right;    // 右孩子
    struct RBTreeNode *parent;    // 父结点
}Node, *RBTree;

// 红黑树的根
typedef struct rb_root{
    Node *node;
}RBRoot;

#endif /* __COMMON_H_ */