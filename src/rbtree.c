#include "rbtree.h"

#include <stdlib.h>

void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *y);
void RB_INSERT_FIXUP(rbtree *t, node_t *z);
void RB_transplant(rbtree *t, node_t *u, node_t *v);
void rb_delete_fixup(rbtree *t, node_t *x);
void delete_rbtree(rbtree *t);
void delete_node_recursive(rbtree *t, node_t *node);
void RB_inorder(const rbtree *t, const node_t *node, key_t *arr, int *index);
node_t *sub_rbtree_min(rbtree *t, node_t *subtree_root);

rbtree *new_rbtree(void) {
    // 앞으로 rbtree t 포인터는 rbtree의 structer를 참조한다
    rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

    // 앞으로 t->nil (nil node)는 note_t를 참조한다.
    t->nil = (node_t *)calloc(1, sizeof(node_t));
    t->nil->parent = t->nil->left = t->nil->right = t->nil;
    t->nil->color = RBTREE_BLACK;
    t->root = t->nil; 
    return t;
}

void RB_transplant(rbtree *t, node_t *u, node_t *v) {
    if (u->parent == t->nil) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;

// scope 개념 때문에 굳이 t,u,v에 대한 변수 명을 후에 맞출 필요 없음

}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *z = calloc(1, sizeof(node_t));
    if (z == NULL){
        return NULL;
    }
    
    z->key = key;
    z->color = RBTREE_RED;
    z->left = z->right = t->nil;

    node_t *y = t->nil;
    node_t *x = t->root;

    while (x != t->nil)
    {
      y = x;
      if (z->key < x->key){
        x = x->left;
      }
       else {
        x = x->right;
      }
    }
    z ->parent = y;

    if (y == t->nil) {
      t->root = z;
    } else if (z->key < y->key) {
      y->left = z;
    } else {
      y->right = z;
    }

  RB_INSERT_FIXUP(t, z);

  return t->root;

}

void left_rotate(rbtree *t, node_t *x) {
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != t->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil) {
        t->root = y;
    } else if (x == x->parent->left) { 
        x->parent->left = y;
    } else { 
        x->parent->right = y;
    }
    y->left = x; 
    x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
    node_t *y = x->left;
    x->left = y->right;
    if (y->right != t->nil) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil) {
        t->root = y;
    } else if (x == x->parent->right) { 
        x->parent->right = y;
    } else { 
        x->parent->left = y;
    }
    y->right = x; 
    x->parent = y;
}

void RB_INSERT_FIXUP(rbtree *t, node_t *z) {
    node_t *y;
    while (z->parent->color == RBTREE_RED) {
        
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        } else {
            y = z->parent->parent->left;
            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}
void delete_node_recursive(rbtree *t, node_t *node) {
    if (node != t->nil) {
        delete_node_recursive(t, node->left);
        delete_node_recursive(t, node->right);
        free(node);
    }
}


void delete_rbtree(rbtree *t) {
    delete_node_recursive(t, t->root);
    free(t->nil);
    free(t);
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t->root;
  while (current != t->nil && key != current->key) {
    if (key < current->key) {
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return (current == t->nil) ? NULL : current; 
}
void rb_delete_fixup(rbtree *t, node_t *x) {
    while (x != t->root && x->color == RBTREE_BLACK ) {
        if (x == x->parent->left) {
            node_t *w = x->parent->right;
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            } else {
                if (w->right->color == RBTREE_BLACK) {
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotate(t, x->parent);
                x = t->root;
            }
        } else {
            node_t *w = x->parent->left;
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            } else {
                if (w->left->color == RBTREE_BLACK) {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
}

// C언어는 C++의 함수 오버로딩이 지원이 안되기 때문에 이렇게 선언함
node_t *sub_rbtree_min(rbtree *t, node_t *subtree_root) {
  node_t *node = subtree_root;
    while (node->left != t->nil) {
        node = node->left;
    }
  return node;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *node = t->root;
    while (node->left != t->nil) {
        node = node->left;
    }
  return node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *node = t->root;
    while (node->right != t->nil) {
        node = node->right;
    }
  return node;
}

int rbtree_erase(rbtree *t, node_t *z) {

    node_t *y = z;
    node_t *x;
    color_t y_original_color = y->color;
    
    if (z->left == t->nil) {
        x = z->right;
        RB_transplant(t, z, z->right);
    } else if (z->right == t->nil) {
        x = z->left;
        RB_transplant(t, z, z->left);
    } else {
        y = sub_rbtree_min(t, z->right);
        y_original_color = y->color;
        x = y->right;
        
        if (y->parent == z) { 
            x->parent = y;
        }
        else {
            RB_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        
        RB_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == RBTREE_BLACK) {
        rb_delete_fixup(t, x);
    }

    free(z); // 동적 메모리 할당 해제
    return 1;
}





int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int index = 0;
  RB_inorder(t, t->root, arr, &index);
  return index;
  // 배열에 key 값을 저장할 때마다 
}

void RB_inorder(const rbtree *t, const node_t *node, key_t *arr, int *index) {
    if (node != t->nil) {
        RB_inorder(t, node->left, arr, index);
        arr[*index] = node->key;
        (*index)++;
        RB_inorder(t, node->right, arr, index);
    }
    // T = nil이 아닐 때 까지 재귀적 호출 진행
    // array의 크기가 정해지지 않은 이유는 입력되는 array의 크기에 따라 변동되기 때문이다.
    // 즉 함수의 사용자에게 메모리 할당과 관리의 책임을 부여
    

}

/* 
rbtree_array(tree,array)
    index = 0 ?
    중위순회(tree,root)
    return index

중위순회(node)
    if node != nil
        중위순회(node.left) 왼쪽 서브 트리 방문
        array[index] = node.key
        index ++
        index = 중위순회 현재 노드 처리
        중위순회(node.right) 오른쪽 서브 트리 방문
        return index

array의 C언어에서는 append가 없단 말이죠
내가 얼마나 넣었는지 index[i]를 기억해야한다.

깊게 순회한 뒤, 필요한것만 memoization을 사용하는 방안

재귀의 결과값을 index로 받고, 다시 돈다?
*/