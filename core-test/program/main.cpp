#include <main>
#include <iostream>

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR message, int nCmdShow) {
int main(void){
	core::Init()(new CoreTest);
	return 0;
}
