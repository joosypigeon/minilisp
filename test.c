#include "object.h"

int main() {
    Object *a = make_symbol("a");
    Object *b = make_symbol("b");
    Object *c = make_symbol("c");

    // Build list (a b c)
    Object *list = cons(a, cons(b, cons(c, make_nil())));

    printf("List: ");
    print_object(list);
    printf("\n");

    // Test car and cdr
    Object *first = car(list);
    Object *second = cadr(list);

    printf("First element (car): ");
    print_object(first);
    printf("\n");

    printf("Second element (cadr): ");
    print_object(second);
    printf("\n");

    // Test type printing
    printf("Type of first: ");
    print_type(first->type);
    printf("\n");

    return 0;
}
