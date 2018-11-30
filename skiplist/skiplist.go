package main

import "fmt"
import "time"
import "math/rand"

/*
*  X------------------>17      level 2 <- top
*  X---->8------------>17->31  level 1
*  X->3->8->9->13->16->17->31  level 0
*/

type Node struct {
    k interface{}
    next   *Node  // nil for the last node
    pre    *Node  // nil for the first node, only convenient for delete op
    bottom *Node  // nil for the last node
}

type SkipList struct {
    nLevel int
    top    int  // top level, point to the topest non-empty sub-list 
    head   []Node
    size   []int  // size at each level 
}

func IsNeedHighLevel() bool {
    i := rand.Intn(2)
    return i == 1 
}

func Create(nLevel int) *SkipList {
    sk := SkipList{}
    sk.nLevel =nLevel
    sk.head = make([]Node, nLevel)
    sk.head[0] = Node{"X", nil, nil, nil}
    for i := 1; i < sk.nLevel; i++ {
        sk.head[i] = Node{"X", nil, nil, &sk.head[i-1]}
    }
    sk.top = -1
    sk.size = make([]int, nLevel)
    return &sk
}

func (sk *SkipList) Insert(val string) bool {
    // top pointer is invalid
    if sk.top < -1 {
        fmt.Printf("[Insert()] ERROR top level = %v\n", sk.top)
        return false
    }

    // sk is empty
    if sk.top == -1 {
        sk.top = 0
        sk.size[0] = 1
        sk.head[0].next = &Node{val, nil, &sk.head[0], nil}
        for i := 1; i < sk.nLevel; i++ {
            if IsNeedHighLevel() == false {
                break
            }
            sk.head[i].next = &Node{val, nil, &sk.head[i], sk.head[i-1].next}
            sk.top = i
            sk.size[i] ++
        }
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
        if pCurNode == nil {
            insertPos[level] = pPreNode
            if level > 0 {
                pPreNode = pPreNode.bottom
                level --
                continue
            } else {
                break
            }
        }
        if pCurNode.k.(string) == val {
            return false
        } else if val > pCurNode.k.(string) {
            pPreNode = pCurNode
            pCurNode = pCurNode.next
        } else {
            insertPos[level] = pPreNode
            if level > 0 {
                pPreNode = pPreNode.bottom
                level --
                continue
            } else {
                break
            }
        }
    }

    // insert from lower level
    pNode := &Node{val, insertPos[0].next, insertPos[0], nil}
    if insertPos[0].next != nil {
        insertPos[0].next.pre = pNode
    }
    insertPos[0].next = pNode
    sk.size[0] ++

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
        sk.size[i] ++
    }
    return true
}

// return (*Node, level)
func (sk *SkipList) Find(val string) (*Node, int) {
    // top pointer is invalid
    if sk.top < -1 {
        fmt.Printf("[Find()] ERROR top level = %v\n", sk.top)
        return nil, -1
    }

    // sk is empty
    if sk.top == -1 {
        return nil, -1
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
                return nil, -1
            }
        }
        if val == pCurNode.k.(string) {
            return pCurNode, level
        } else if val > pCurNode.k.(string) {
            pPreNode = pCurNode
        } else {
            if level > 0 {
                pPreNode= pPreNode.bottom
                level--
                continue
            } else {
                return nil, -1
            }
        }
    }
    return nil, -1 // never reach
}

func (sk *SkipList) Delete(val string) bool {
    node, level := sk.Find(val)  // node at top level
    if node == nil {
        return false
    }

    for {
        pPreNode := node.pre
        pPostNode := node.next

        pPreNode.next = pPostNode
        if pPostNode != nil {
            pPostNode.pre = pPreNode
        }

        sk.size[level] --
        if sk.size[level] == 0 {
            sk.top --
        }

        // delete this node, move to lower level
        if node.bottom != nil {
            node = node.bottom
            level --
        } else {
            node = nil
            break
        }
    }
    return true
}

func (sk *SkipList) Show() {
    fmt.Printf("skip list top = %v\n", sk.top)

    for i := 0; i <= sk.top; i++ {
        fmt.Printf("Level: %v, size: %v\n", i, sk.size[i])
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
    sk := Create(10)

    fmt.Println("===== Insert: =====")
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

    fmt.Println("===== Find: =====")
    fmt.Println(sk.Find("100"))
    fmt.Println(sk.Find("1000"))
    fmt.Println(sk.Find("-9"))
    fmt.Println(sk.Find("18"))
    fmt.Println(sk.Find("-100"))

    fmt.Println("===== Delete: =====")
    sk.Delete("18")
    sk.Delete("1")
    sk.Delete("-1")
    sk.Delete("-9")
    sk.Delete("1000")
    sk.Show()

    fmt.Println("===== Insert: =====")
    sk.Insert("-10")
    sk.Insert("9")
    sk.Insert("61")
    sk.Insert("13")
    sk.Insert("100")
    sk.Insert("17")
    sk.Insert("8")
    sk.Show()
}