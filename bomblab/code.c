char line[] = "maduiersnfotvbylSo?you?think?you?can?stop?the?bomb?with?ctrl-c,?do?you?"; //0x4024b0
char aaa[] = "flyers";                                                                   //0x40245e
void phase_5(char input[])
{
    if (strlen(input) != 5)
        explode();
    else
    {
        char wtf[6];                       //rsp+10
        for (int i = 0; i < 6; ++i)        //I in rax
            wtf[i] = line[input[i] & 0xf]; //input in rbx
        if (strcmp(wtf, aaa))
            explode();
    }
}
struct node{
    int n;
    struct node *left,*right;
};
int fun7(int n,struct node *ptr){
    if(0==ptr)
        return 0xffffffff;
    else{
        if(n<=ptr->n)
            if(n==ptr->n)
                return 0;
            else
                return 2*fun7(n,ptr->left);
        else
            return 2*fun7(n,ptr->right)+1;
    }
}