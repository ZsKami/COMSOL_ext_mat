// __declspec(dllexport) is needed to export the function from the DLL.
__declspec(dllexport) int add(int a, int b) {
    return a + b;
}
