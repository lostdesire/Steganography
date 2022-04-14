#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <vector>
#include <math.h>
using namespace std;

int main(int argc, char* argv[]) {
	FILE* fin;
	FILE* fout;
	string e;
	string text;

	try {
	// encoding mode
		if (strcmp(argv[1], "e") == 0) {						
			fopen_s(&fin, "origin.bmp", "rb+");
			cout << endl;
			if (fin) {
				getline(cin, text);								// �ؽ�Ʈ �б� ('\n'�� ����)

				vector<bitset<8>> text_to_ascii;

				// �ؽ�Ʈ 2���� �ƽ�Ű�� ��ȯ
				for (const auto& item : text) {
					text_to_ascii.push_back(item);
				}

				// �ؽ�Ʈ�� �ƽ�Ű ���� ��(2����)
				// Little Endian ������� ����
				bitset<12> text_size(text_to_ascii.size());

				char info[54];
				fread(info, sizeof(char), 54, fin);				// bmp ���� ����κ�

				int size = *(int*)&info[2];						// bmp ���� ũ��
				char* data = new char[size];
				fread(data, sizeof(char), size, fin);			// bmp ������ �����ͺκ�

				fclose(fin);

				// �ؽ�Ʈ�� ����� �������� ù 12����Ʈ�� ���
				if (size > 12) {
					for (int i = 0; i < 12; i++) {
						if ((int)data[i] % 2 == 0) {
							if (text_size.test(11 - i)) {
								data[i]++;
							}
						}
						else {
							if (!text_size.test(11 - i)) {
								data[i]++;
							}
						}
					}
				}
				else {
					e = "data size error";						// data size�� �ùٸ��� ���� ��
					throw e;
				}
				
				// �ؽ�Ʈ�� ������ ��ŭ �����͸� ���
				for (unsigned int i = 0; i < text_to_ascii.size(); i++) {
					for (int j = 0; j < 8; j++) {
						if ((int)data[i * 8 + j + 12] % 2 == 0) {		// 8����Ʈ �� �ؽ�Ʈ 1���� ���
							if (text_to_ascii.at(i).test(7 - j)) {		// �ؽ�Ʈ ������ 12����Ʈ ���ķ� ���
								data[i * 8 + j + 12]++;
							}
						}
						else {
							if (!text_to_ascii.at(i).test(7 - j)) {
								data[i * 8 + j + 12]++;
							}
						}
					}
				}

				// stego.bmp ����
				fopen_s(&fout, "stego.bmp", "wb");

				if (fout) {
					fwrite(info, sizeof(char), 54, fout);				// bmp ���� ��� �Է�
					fwrite(data, sizeof(char), size, fout);				// bmp ���� ������ �Է�
				}
				else {
					e = "can't create 'stego.bmp' file";				// stego.bmp ���� �Ұ��� �� ��
					throw e;
				}

				fclose(fout);
			}
			else {
				e = "no exist 'origin.bmp' file";						// origin.bmp ������ �������� ���� ��
				throw e;
			}
		}
	// decoding mode
		else if (strcmp(argv[1], "d") == 0) {
			fopen_s(&fin, "stego.bmp", "rb");
			if (fin) {
				char info[54];
				fread(info, sizeof(char), 54, fin);				// bmp������ ����κ�

				int size = *(int*)&info[2];						// bmp������ ũ��
				char* data = new char[size];
				fread(data, sizeof(char), size, fin);			// bmp������ �����ͺκ�

				fclose(fin);

				// bmp ���Ͽ� ������ �ؽ�Ʈ�� ����
				int stego_size = 0;
				if (size > 12) {
					for (int i = 0; i < 12; i++) {
						stego_size = stego_size * 2 + (int)data[i] % 2;
					}
				}
				else {
					e = "data size error";						// data size�� �ùٸ��� ���� ��
					throw e;
				}
				

				vector<char> stego_text;

				int stego_temp = 0;
				for (int i = 0; i < stego_size; i++) {
					// �� ����Ʈ�� �� ������ ��Ʈ 8����Ʈ ������ ����
					for (int j = 0; j < 8; j++) {
						stego_temp = stego_temp * 2 + ((int)data[i * 8 + j + 12] % 2);
					}
					// ���� �Ѱ� ����
					stego_text.push_back((char)stego_temp);
				}

				// bmp ���Ͽ� ������ ���ڿ� ���
				cout << endl;
				for (const auto& item : stego_text) {
					cout << item;
				}
				cout << endl;
			}
			else {
				e = "no exist 'stego.bmp' file";		// stego.bmp ������ �������� ���� ��
				throw e;
			}
		
		}
		else {
			e = "invalid option";						// ��ȿ���� ���� �ɼ��� �������� ��
			throw e;
		}
	}
	catch (string e) {									// ���� �߻��� ���� ���
		cout << "\n[Error] : " << e << endl;
		cout << "Please re-run Program" << endl;
	}

	return 0;
}