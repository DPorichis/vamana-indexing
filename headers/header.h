// Nothing to see here

struct node
{
    int d_count; // object dimension
    void* components;

    int n_count;
    struct node* neighbours;
};
