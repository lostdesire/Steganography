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
				getline(cin, text);					// 텍스트 읽기 ('\n'로 구분)

				vector<bitset<8>> text_to_ascii;

				// 텍스트 2진수 아스키로 변환
				for (const auto& item : text) {
					text_to_ascii.push_back(item);
				}

				// 텍스트의 아스키 문자 수(2진수)
				// Little Endian 방식으로 저장
				bitset<12> text_size(text_to_ascii.size());

				char info[54];
				fread(info, sizeof(char), 54, fin);			// bmp 파일 헤더부분

				int size = *(int*)&info[2];				// bmp 파일 크기
				char* data = new char[size];
				fread(data, sizeof(char), size, fin);			// bmp 파일의 데이터부분

				fclose(fin);

				// 텍스트의 사이즈를 데이터의 첫 12바이트에 기록
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
					e = "data size error";						// data size가 올바르지 않을 때
					throw e;
				}
				
				// 텍스트의 사이즈 만큼 데이터를 기록
				for (unsigned int i = 0; i < text_to_ascii.size(); i++) {
					for (int j = 0; j < 8; j++) {
						if ((int)data[i * 8 + j + 12] % 2 == 0) {		// 8바이트 당 텍스트 1문자 기록
							if (text_to_ascii.at(i).test(7 - j)) {		// 텍스트 내용은 12바이트 이후로 기록
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

				// stego.bmp 생성
				fopen_s(&fout, "stego.bmp", "wb");

				if (fout) {
					fwrite(info, sizeof(char), 54, fout);				// bmp 파일 헤더 입력
					fwrite(data, sizeof(char), size, fout);				// bmp 파일 데이터 입력
				}
				else {
					e = "can't create 'stego.bmp' file";				// stego.bmp 생성 불가능 할 때
					throw e;
				}

				fclose(fout);
			}
			else {
				e = "no exist 'origin.bmp' file";					// origin.bmp 파일이 존재하지 않을 때
				throw e;
			}
		}
	// decoding mode
		else if (strcmp(argv[1], "d") == 0) {
			fopen_s(&fin, "stego.bmp", "rb");
			if (fin) {
				char info[54];
				fread(info, sizeof(char), 54, fin);		// bmp파일의 헤더부분

				int size = *(int*)&info[2];			// bmp파일의 크기
				char* data = new char[size];
				fread(data, sizeof(char), size, fin);		// bmp파일의 데이터부분

				fclose(fin);

				// bmp 파일에 숨겨진 텍스트의 길이
				int stego_size = 0;
				if (size > 12) {
					for (int i = 0; i < 12; i++) {
						stego_size = stego_size * 2 + (int)data[i] % 2;
					}
				}
				else {
					e = "data size error";			// data size가 올바르지 않을 때
					throw e;
				}
				

				vector<char> stego_text;

				int stego_temp = 0;
				for (int i = 0; i < stego_size; i++) {
					// 각 바이트의 맨 마지막 비트 8바이트 단위로 추출
					for (int j = 0; j < 8; j++) {
						stego_temp = stego_temp * 2 + ((int)data[i * 8 + j + 12] % 2);
					}
					// 문자 한개 저장
					stego_text.push_back((char)stego_temp);
				}

				// bmp 파일에 숨겨진 문자열 출력
				cout << endl;
				for (const auto& item : stego_text) {
					cout << item;
				}
				cout << endl;
			}
			else {
				e = "no exist 'stego.bmp' file";		// stego.bmp 파일이 존재하지 않을 때
				throw e;
			}
		
		}
		else {
			e = "invalid option";					// 유효하지 않은 옵션을 선택했을 때
			throw e;
		}
	}
	catch (string e) {							// 에러 발생시 에러 출력
		cout << "\n[Error] : " << e << endl;
		cout << "Please re-run Program" << endl;
	}

	return 0;
}
