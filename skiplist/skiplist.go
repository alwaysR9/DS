package main

import "fmt"
import "time"
import "math/rand"

/*
*  X------------------>17      level 2 <- top
*  X---->8------------>17->31  level 1
*  X->3->8->9->13->16->17->31  level 0
*
*  lookup: 100
*  lookup: 17
*  lookup: 8
*  lookup: 16
*
*  insert: 8
*  insert: 10
*  insert: 32
*  
*  delete: 100
*  delete: 8
*  delete: 16
*/

type Node struct {
    k interface{}
    next   *Node  // nil for the last node
    pre    *Node  // nil for the first node, only convenient for delete op
    bottom *Node  // nil for the last node
}

type SkipList struct {
    nLevel int
    top    int // top level
    head   []Node
    size   int
}

func IsNeedHighLevel() bool {
    i := rand.Intn(2)
    return i == 1 
}

func Create(nLevel int) *SkipList {
    sk := SkipList{}
    sk.nLevel =nLevel
    sk.head = make([]Node, nLevel)
    sk.top = 0
    sk.size = 0
    sk.head[0] = Node{"X", nil, nil, nil}
    for i := 1; i < sk.nLevel; i++ {
        sk.head[i] = Node{"X", nil, nil, &sk.head[i-1]}
    }
    return &sk
}

func (sk *SkipList) Insert(val string) bool {
    // sk is empty
    if sk.head[sk.top].next == nil {
        sk.head[0].next = &Node{val, nil, &sk.head[0], nil}
        for i := 1; i < sk.nLevel; i++ {
            if IsNeedHighLevel() == false {
                break
            }
            sk.head[i].next = &Node{val, nil, &sk.head[i], sk.head[i-1].next}
            if i > sk.top {
                sk.top = i
            }
        }
        sk.size = 1
        return true
    }

    // sk is not empty
    insertPos := make([]*Node, sk.nLevel)  // insert position at each level
    pPreNode := &(sk.head[sk.top])
    level := sk.top

    for i := 0; i < sk.nLevel; i++ {
        insertPos[i] = &sk.head[i]
    }

    for {
        pCurNode := pPreNode.next
        // val > pPreNode.k, move to next level
        if pCurNode == nil {
            insertPos[level] = pPreNode
            if level > 0 {
                pPreNode = pPreNode.bottom
                level--
            } else {
                break
            }
            continue
        }
        // val already in skip list
        if pCurNode.k.(string) == val {
            return false
        }
        // val < cur.k, move to next level
        if val < pCurNode.k.(string) {
            insertPos[level] = pPreNode
            if level > 0 {
                pPreNode = pPreNode.bottom
                level--
            } else {
                break
            }
            continue
        }
        // val > cur.k
        if val > pCurNode.k.(string) {
            pPreNode = pCurNode
            pCurNode = pCurNode.next
        }
    }

    // insert from lower level
    pNode := &Node{val, insertPos[0].next, insertPos[0], nil}
    if insertPos[0].next != nil {
        insertPos[0].next.pre = pNode
    }
    insertPos[0].next = pNode


    for i := 1; i < sk.nLevel; i++ {
        if IsNeedHighLevel() == false {
            break
        }
        pNode = &Node{val, insertPos[i].next, insertPos[i], pNode}
        if insertPos[i].next != nil {
            insertPos[i].next.pre = pNode
        }
        insertPos[i].next = pNode
        if i > sk.top {
            sk.top = i
        }
    }
    sk.size ++
    return true
}

func (sk *SkipList) Find(val string) *Node {
    // sk is empty
    if sk.size == 0 {
        return nil
    }

    // sk is not empty
    level := sk.top
    pPreNode := &sk.head[level]
    for {
        pCurNode := pPreNode.next
        if pCurNode == nil {
            if level > 0 {
                pPreNode= pPreNode.bottom
                level--
                continue
            } else {
                return nil
            }
        }
        if val == pCurNode.k.(string) {
            return pCurNode
        } else if val > pCurNode.k.(string) {
            pPreNode = pCurNode
        } else {
            if level > 0 {
                pPreNode= pPreNode.bottom
                level--
                continue
            } else {
                return nil
            }
        }
    }
    return nil // never reach
}

func (sk *SkipList) Show() {
    fmt.Printf("skip list size = %v\n", sk.size)

    for i := 0; i <= sk.top; i++ {
        fmt.Printf("Level: %v\n", i)
        pCurNode := &sk.head[i]
        for pCurNode != nil {
            fmt.Printf("%v ", pCurNode.k.(string))
            pCurNode = pCurNode.next
        }
        fmt.Printf("\n")
    }
}

func main() {
    rand.Seed(time.Now().UnixNano())
    sk := Create(4)
    sk.Insert("1")
    sk.Insert("2")
    sk.Insert("7")
    sk.Insert("3")
    sk.Insert("3")
    sk.Insert("4")
    sk.Insert("18")
    sk.Insert("0")
    sk.Insert("-9")
    sk.Insert("11")
    sk.Insert("100")
    sk.Insert("7")
    sk.Insert("10")
    sk.Show()

    fmt.Println(sk.Find("100"))
    fmt.Println(sk.Find("1000"))
    fmt.Println(sk.Find("-9"))
    fmt.Println(sk.Find("18"))
    fmt.Println(sk.Find("-100"))
}