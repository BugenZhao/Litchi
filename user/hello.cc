//
// Created by Bugen Zhao on 7/13/20.
//

int add(int a, int b) {
    return a + b;
}

int main() {
    add(*(int *) 4, 4);
    return 0;
}