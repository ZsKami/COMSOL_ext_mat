# 如何编写自定义的外部材料库

.c

.dll

接口类型（Interface Type）：
- 广义应力-应变关系
- 广义应力-变形关系
- 非弹性残余应变
- 非弹性残余变形


以下三个案例均定义广义应力-应变关系：  
共有：
- e，Green-Lagrange strain tensor
- s，Second Piola-Kirchhoff stress 
- D，Jacobian
- nPar，模型参数个数
- Par，模型参数具体值

其中，nStates 的意义：  



## case1. Linear_elastic
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

## case2. Linear_hardening
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

## case3. Mazars_damage
```c
EXPORT int eval(double e[6],       // Input: Green-Lagrange strain tensor components in Voigt order (xx,yy,zz,yz,zx,xy)
                double s[6],       // Output: Second Piola-Kirchhoff stress components in Voigt order (xx,yy,zz,yz,zx,xy)
                double D[6][6],    // Output: Jacobian of stress with respect to strain, 6-by-6 matrix in row-major order
                int *nPar,         // Input: Number of material model parameters, scalar,
                double *par,       // Input: Parameters: par[0] = E0, par[1] = nu0, ...
                int *nStates,      // Input: Number of states, scalar               
                double *states) {:}  // States, nStates-vector
```
