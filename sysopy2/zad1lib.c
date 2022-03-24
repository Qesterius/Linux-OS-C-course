void writeft(FILE* from, FILE* to)
{
    char line[255]="";
    char buffor[255]="";
    char trash[255]=""
    int diff =0;

    int areCharactersInside=0;
    while(fread(buffor,255,from)>0)
    {
        
        for(int i=0;i<255;i++)
        {
            if(buffor[i]==EOF)
                line[i-diff] = buffor[i];
                if(areCharactersInside==1)
                {
                    if(trash[0]!="\0")
                        fwrite(to,1,sizeof(trash),trash);
                    fwrite(to,1,sizeof(line),line);
                    
                }    //dopisz do pliku to // if areCharactersInside
                line="";
                trash="";
                break;
            else if (buffor[i]=="\n")
            {
                line[i-diff] = buffor[i];
                if(areCharactersInside==1)
                {
                    if(sizeof(trash)>1)
                        fwrite(to,1,sizeof(trash),trash);
                    fwrite(to,1,sizeof(line),line);
                    
                }    //dopisz do pliku to // if areCharactersInside
                line="";
                trash="";
                diff = i+1;
            }
            else if(buffor=>33 && buffor[i]<=126)
            {
                areCharactersInside=1;
                line[i-diff] = buffor[i];
            }
        }
        trash = line;
        line=""
        areCharactersInside=0;
    }




}

int main(int argc, char *argv[])
{   
    FILE* from;
    FILE* to;
    char* path;
    if(argc==0)
    {
        
        printf("Podaj sciezke do pliku do ktorego bedziemy dopisywac:");
        do{

            scanf("%s",path);
            from = fopen(path,"w+");//mode??
            if(!from)printf("Sciezka %s jest nieprawidłowa\n",path);
      
        }while(!from)

         printf("Podaj sciezke do pliku z ktorego bedziemy kopiowac:");
        do{

            scanf("%s",path);
            to = fopen(path,"w+");//mode??
            if(!to)printf("Sciezka %s jest nieprawidłowa\n",path);
      
        }while(!to)
        
    }
    else
    {

        from = fopen(arg[1],"w+");//mode??
        to = fopen(arg[2],"w+");//mode??
        if(!from || !to)
        {
            printf("Sciezki %s %s są nieprawidłowe\n",arg[1],arg[2]);
            printf("from: arg1, to: arg2 \n")
            return 0;
        }
    }
    writeft(from,to);
    close(from);
    close(to);

        
    return 0;

}
