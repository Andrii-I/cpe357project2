typedef unsigned char byte;

void main()
{
    byte* a;
    unsigned char b = 'b';
    a = &b;
    printf("%i\n", ((int)(*a)));
}