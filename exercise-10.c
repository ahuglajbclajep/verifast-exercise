#include <stdlib.h>
#include <assert.h>

struct node {
    int value;
    struct node* next;
};
typedef struct node node_t;

struct stack {
    node_t* head;
};
typedef struct stack stack_t;

/*@
inductive ints = ints_nil | ints_cons(int, ints);

fixpoint int ints_head(ints values) {
    switch (values) {
        case ints_nil: return 0;
        case ints_cons(value, values0): return value;
    }
}

fixpoint ints ints_tail(ints values) {
    switch (values) {
        case ints_nil: return ints_nil;
        case ints_cons(value, values0): return values0;
    }
}

predicate nodes(node_t* node, ints values) =
    !node ? values == ints_nil : node->value |-> ?value &*&
    node->next |-> ?next &*& malloc_block_node(node) &*&
    nodes(next, ?values0) &*& values == ints_cons(value, values0);

predicate stack(stack_t* stack, ints values) =
    stack->head |-> ?head &*& malloc_block_stack(stack) &*& nodes(head, values);
@*/

stack_t* create_stack()
    //@ requires true;
    //@ ensures  stack(result, ints_nil);
{
    stack_t* stack = malloc(sizeof(stack_t));
    if (!stack) abort();
    stack->head = 0;
    //@ close nodes(0, ints_nil);
    //@ close stack(stack, ints_nil);

    return stack;
}

void stack_push(stack_t* stack, int value)
    //@ requires stack(stack, ?values);
    //@ ensures  stack(stack, ints_cons(value, values));
{
    node_t* new_node = malloc(sizeof(node_t));
    if (!new_node) abort();
    new_node->value = value;

    //@ open stack(stack, values);
    new_node->next = stack->head;
    stack->head = new_node;
    //@ close nodes(new_node, ints_cons(value, values));
    //@ close stack(stack, ints_cons(value, values));
}

int stack_pop(stack_t* stack)
    //@ requires stack(stack, ?values) &*& values != ints_nil;
    //@ ensures  stack(stack, ints_tail(values)) &*& result == ints_head(values);
{
    //@ open stack(stack, values);
    node_t* head = stack->head;
    //@ open nodes(head, values);
    int value = head->value;

    stack->head = head->next;
    free(head);
    //@ close stack(stack, ints_tail(values));

    return value;
}

void stack_dispose(stack_t* stack)
    //@ requires stack(stack, ints_nil);
    //@ ensures  true;
{
    //@ open stack(stack, ints_nil);
    //@ open nodes(_, _);
    free(stack);
}

int main()
    //@ requires true;
    //@ ensures  true;
{
    stack_t* stack = create_stack();
    stack_push(stack, 10);
    stack_push(stack, 20);

    int result1 = stack_pop(stack);
    assert(result1 == 20);

    int result2 = stack_pop(stack);
    assert(result2 == 10);

    stack_dispose(stack);

    return 0;
}
