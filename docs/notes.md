# 如何编写自定义的外部材料库
## 步骤
### 写源代码并编译
源码以 c 或 fortran 语言写成 .c 或 .f 等文件，利用编译器编译为 .dll 文件，以 c 语言为例：
gcc -shared -o file_name.dll src_name.c
### DLL 导入 COMSOL
在全局材料中，建立外部材料。
1. 导入模型的库
2. 接口类型选择： 一般为广义应力-应变关系
```
接口类型（Interface Type）：
- 广义应力-应变关系
- 广义应力-变形关系
- 非弹性残余应变
- 非弹性残余变形
```
3. 状态基本名称和状态数（对应程序中的`int *nStates1`、`double *states1`等）
若有初始值，进行初始化。
4. 材料属性明细/属性组定义
分为两类，基本(def)和广义应力-应变关系（comcomsolgeneralStressStrain），后者的材料模型参数对应`int *nPar`、`double *par`。
> 注意：模型参数与状态变量不同。模型参数为定值，状态变量包含初始值和程序中自定义的更新算法。

## 案例分析
以下三个案例均定义广义应力-应变关系：  
共有：
- e，Green-Lagrange strain tensor
- s，Second Piola-Kirchhoff stress 
- D，Jacobian
- nPar，模型参数个数
- Par，模型参数具体值

其中，nStates 的意义：  



### case1. Linear_elastic
定义：
```c
EXPORT int eval(double e[6],         // Input: Green-Lagrange strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
                double s[6],         // Output: Second Piola-Kirchhoff stress components in Voigt order (xx,yy,zz,yz,zx,xy)
                double D[6][6],         // Output: Jacobian of stress with respect to strain, 6-by-6 matrix in row-major order
                int *nPar,         // Input: Number of material model parameters, scalar
                double *par,       // Input: Parameters: par[0] = E, par[1] = nu
                int *nStates,      // Input: Number of states, scalar        
                double *states) {;}  // States, nStates-vector
```

### case2. Linear_hardening
```c
EXPORT int eval(double e[6],         // Input: Green-Lagrange strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
                double s[6],         // Output: Second Piola-Kirchhoff stress components in Voigt order (xx,yy,zz,yz,zx,xy)
                double Jac[6][6],    // Output: Jacobian of stress with respect to strain, 6-by-6 matrix in row-major order
                int *nPar,           // Input: Number of material model parameters, scalar
                double *par,         // Input: Parameters: par[0] = E, par[1] = nu, par[2] = sigyYs0, par[3] = ETiso
                int *nStates1,       // Input: Number of states, scalar        
                double *states1,     // Internal state: Plastic strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
				int *nStates2,       // Input: Number of states, scalar        
                double *states2,     // Internal state: Accumulated effective plastic strain
				int *nStates3,       // Input: Number of states, scalar        
                double *states3) {;}   // Internal state: Plastic work
```

### case3. Mazars_damage
```c
EXPORT int eval(double e[6],       // Input: Green-Lagrange strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
                double s[6],       // Output: Second Piola-Kirchhoff stress components in Voigt order (xx,yy,zz,yz,zx,xy)
                double D[6][6],    // Output: Jacobian of stress with respect to strain, 6-by-6 matrix in row-major order
                int *nPar,         // Input: Number of material model parameters, scalar,
                double *par,       // Input: Parameters: par[0] = E0, par[1] = nu0, ...
                int *nStates,      // Input: Number of states, scalar               
                double *states) {:}  // States, nStates-vector
```
