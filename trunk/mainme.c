#include <stdio.h>
#include <stdlib.h>

typedef struct pinyin
{
	char strPY[7];
	char strHZ[2];
	struct pinyin *next;
}py;

typedef char *sougou;

/*Load gbkpy.org to chain table.*/
py *load_basepy(FILE *fpbs)
{
	char ch;
	int n = 0,i;
	py *pointer,*header;
	header = (py*)malloc(sizeof(py));
	pointer = header;

	while(1)
	{
		if(n > 0)
			fgetc(fpbs);
		py *table = (py*)malloc(sizeof(py));
		for(i = 0;(ch = fgetc(fpbs)) != 0x20;i++)
		{
			if(ch == EOF)
			{
				free(table);
				return header;
			}
			table->strPY[i] = ch;
		}
		table->strPY[i] = '\0';
		table->strHZ[0] = fgetc(fpbs);
		table->strHZ[1] = fgetc(fpbs);
		/*table->strHZ[2] = '\0';*/
		table->next = NULL;
		pointer->next = table;
		pointer = table;
		n++;
	}
}

void print_basepy(py *header)
{
	py *pointer = header;
	while(pointer)
	{
		printf("%s,",pointer->strPY);
		printf("%s\n",pointer->strHZ);
		pointer = pointer->next;
	}
}

/*Free gbkpy.org's chain table.*/
void free_basepy(py *header)
{
	py *pointer;
	while(header)
	{
		pointer = header->next;
		free(header);
		header = pointer;
	}
}

/*Read one line a time from sougou cell wordstock.*/
sougou sougou_readline(FILE *fpsg)
{
	char ch;
	int i;
	sougou sg = (sougou)malloc(200*sizeof(char));

	for(i = 0;(ch = getc(fpsg)) != '\n';i++)
	{
		if(ch == EOF)
			return NULL;
		sg[i] = ch;
	}
	sg[i] = '\0';
	return sg;
}

/*Drop pinyin from current line.*/
void sougou_drop_py(sougou sg)
{
	int i = 0,n = 0;
	char tmp[200] = "";
	while(sg[i] != '\0')
	{
		if(sg[i] <= 96 || sg[i] >= 123)
		{	
			tmp[n++] = sg[i++];
			tmp[n++] = sg[i++];
		}
		else
			i++;
	}
	tmp[n] = '\0';
	for(i =0;tmp[i] != '\0';i++)
		sg[i] = tmp[i];
	sg[i] = '\0';
}

/*Generate the fcitx form wordstock.*/
void generate_org(FILE *fpsg,FILE *fpbs)
{
	int i = 0;
	sougou sg;
	py *py_header,*ptr;
	py_header = load_basepy(fpbs);

	while((sg = sougou_readline(fpsg)) != NULL)
	{
		i = 0;
		while(sg[i] != '\0')
		{
			ptr = py_header;
			while(sg[i] != ptr->strHZ[0] || sg[i+1] != ptr->strHZ[1])
				ptr = ptr->next;/*If can't find the matched,a segmentation fault will come.*/

			if(sg[i+2] < 96 || sg[i+2] > 123)
				printf("%s",ptr->strPY);
			for(;sg[i+2] > 96 && sg[i+2] < 123;i++)
				printf("%c",sg[i+2]);
			if(sg[i+2] != '\0')
				printf("\'");
			i = i + 2;
		}
		sougou_drop_py(sg);
		printf(" %s\n",sg);
		free(sg);
	}
	free_basepy(py_header);
}

int main(int argc,char *argv[])
{
	FILE *fpbs,*fpsg;
	if(argv[1] == NULL)
	{
		printf("\nUseage:sg2fcitx sougouCellStock.txt > fcitxStyle.txt\n");
		return 0;
	}
	if((fpbs = fopen("gbkpy.org","r")) == NULL || (fpsg = fopen(argv[1],"r")) == NULL)
	{
		printf("\nMaybe lack of gbkpy.org.\n");
		return 0;
	}
	else
	{
		generate_org(fpsg,fpbs);
		fclose(fpsg);
		fclose(fpbs);
	}
	return 0;
}
