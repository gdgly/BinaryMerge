#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#pragma warning(disable:6031)

#define MAX_PATH 256

void help(int argc);
bool combin_file(char* loader_path, char* app_path, char* output_path, unsigned long offset_addr);

bool str_to_hex(char *pch, unsigned long *p_addr)
{
	if (NULL == pch)
		return false;
	char* p = pch;
	const char hex[] = "0123456789ABCDEF";

	if (*p == '0' && (*(p+1) == 'x' || *(p+1) == 'X'))
		p += 2;

	int index = 0;
	while ('\0' != *p)
	{
		for (int i = 0; i < 16; i++)
		{
			if (*p == hex[i])
			{
				*p_addr = (*p_addr << 4) | i;
				break;
			}
		}
		index++;
		++p;
	}
	return true;
}

bool get_filename(char* path, char *filename)
{
	if (NULL == path)
		return false;

	char *pos;
	int name_len;
	name_len = strlen(path);
	pos = path + name_len;
	while (*pos != '\\' && pos != path)
		pos--;

	if (pos == path)
	{
		filename = path + 1;
		return true;
	}
	name_len = name_len - (pos - path);

	memcpy(filename, pos + 1, name_len);

	return true;
}

bool remove_sufix(char* pfile)
{
	char *p = strchr(pfile, '.');
	if (NULL == p)
		return false;

	*p = '\0';
	return true;
}

int main(int argc, const char** argv)
{
	system("color 09");
	char loader_path[MAX_PATH] = { 0 };
	char app_path[MAX_PATH] = { 0 };
	char offset_addr[MAX_PATH] = { 0 };
	char loadername[MAX_PATH] = { 0 }, appname[MAX_PATH] = { 0 };
	char outputname[MAX_PATH * 2 + 1] = { 0 };
	unsigned long addr = 0;
	bool ret = false;

	printf("������loader�ļ����ƣ���·������APP�������ƣ��Կո�����APP��ַ��hex��ʽ�磺0x4000����\n");
	scanf("%s%s%s", loader_path, app_path, offset_addr);
	
	ret = str_to_hex(offset_addr, &addr);
	if (false == ret)
		return -1;

	ret = get_filename(loader_path, loadername);
	if (false == ret)
		return -1;

	ret = get_filename(app_path, appname);
	if (false == ret)
		return -1;

	ret = remove_sufix(appname);
	if (false == ret)
		return -1;

	ret = remove_sufix(loadername);
	if (false == ret)
		return -1;

	sprintf(outputname, "./%s(%s).bin", appname, loadername);

	ret = combin_file(loader_path, app_path, outputname, addr);
	if (false == ret)
	{
		printf("\r\n�ļ��ϳ�ʧ�ܣ�\r\n");
		system("pause");
		return -1;
	}

	printf("\r\n����ɹ����ļ�·����%s\r\n", outputname);
	system("pause");

	return 0;
}

void help(int argc)
{
	printf("Usrage:\n");
}

bool combin_file(char* loader_path, char* app_path, char* output_path, unsigned long offset_addr)
{
	if (NULL == loader_path || NULL == app_path || NULL == output_path)
		return false;

	FILE* fp_loader = fopen(loader_path, "rb");
	if (fp_loader == NULL)
		return false;

	FILE* fp_app = fopen(app_path, "rb");
	if (fp_app == NULL)
		return false;

	FILE* fp_dest = fopen(output_path, "wb");
	if (fp_dest == NULL)
		return false;

	fseek(fp_loader, SEEK_SET, SEEK_END);
	unsigned long loader_size = ftell(fp_loader);
	rewind(fp_loader);

	fseek(fp_app, SEEK_SET, SEEK_END);
	unsigned long app_size = ftell(fp_app);
	rewind(fp_app);

	unsigned char* pbuf = (unsigned char*)malloc(sizeof(unsigned char) * loader_size);
	if (NULL != pbuf)
	{
		// д��loader
		fread(pbuf, sizeof(unsigned char), loader_size * sizeof(unsigned char), fp_loader);
		fwrite(pbuf, sizeof(unsigned char), loader_size * sizeof(unsigned char), fp_dest);
		// д��ʣ���ֽ�Ϊ0XFF
		unsigned char ch = 0xff;
		for(size_t i = 0; i < offset_addr-loader_size; i++)
			fwrite(&ch, sizeof(unsigned char), 1, fp_dest);

		// д��APP
		free(pbuf);
		pbuf = (unsigned char*)malloc(sizeof(unsigned char) * app_size);
		if (pbuf != NULL)
		{
			fread(pbuf, sizeof(unsigned char), app_size * sizeof(unsigned char), fp_app);
			fwrite(pbuf, sizeof(unsigned char), app_size * sizeof(unsigned char), fp_dest);
			free(pbuf);
		}
	}
	fclose(fp_loader);
	fclose(fp_app);
	fclose(fp_dest);

	return true;
}
