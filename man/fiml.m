
<Full Information Maximum Likelihood (FIML)>
Full Information Maximum Likelihood (FIML)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Note : the method described below was implemented in KaA but is not yet available in IODE.

Under the assumption that (u~-1t~= , ..., u~-mt~=) is independently and identically distributed
as multivariate N(O,S), the density function for one observation is

.~a"kaa8.jpg"

where S~+*~= = S~+-1~= and S~-ij~= ~+* ~= is the ij element of S~+*~=.
The Jacobian J~-t~= has been defined before.

The likelihood function of the sample t=1,...,T is

.~a"kaa9.jpg"

and the log of L* is

.~a"kaa10.jpg"


Since log L* is a monotonic function of L*, maximizing log L* is equivalent to maximizing L*.

The problem of maximizing log L* can be broken into two parts: the first is to
maximize log L* with respect to the elements of S*, and the second is to substitute the
resulting expression for S* into (11) and to maximize this "concentrated" likelihood
function with respect to a. The derivative of log L* with respect to s~-ij~=~+*~= is

~a"kaa11.jpg"

where s~-ij~=  is the ij element of S~+*-1~=. This derivative uses the fact that ~a"kaa_17.jpg" = a~+*~= ~-ij~=
for a matrix A. Setting (12) equal to zero and solving for s~+*~= ~-ij~== yields

~a"kaa12.jpg"

since ~a"kaa_18.jpg" and therefore ~a"kaa_19.jpg". Substituting (13) into (11) yelds.

~a"kaa13.jpg"

The Tm/2 term comes from the fact that ~a"kaa_20.jpg" ~a"kaa_21.jpg". The first and last terms on the RHS of (14)
are constants, and thus the expression to be maximized with respect to a consists ofjust
the middle two terms. Since log|S~+*~=| = log|S~+-1~=| = -log|S|, the function to be maximized
can be written

~a"kaa14.jpg"

where, as noted earlier, the ij element of S, S~-ij~=, is ~a"kaa_23.jpg".

FIML estimates
of a are thus obtained by maximazing L with respect to a. An estimate of the covariance
matrix of these estimates (say V^~-4~=) is :

~a"kaa15.jpg"

where the derivatives are evaluated at the optimum.

Phillips (1982) has pointed out that Amemiya's proof of consistency and asymptotic
efficiency (1977) is based on an incorrect lemma. This is corrected in a later paper
(Amemiya Ig82).Amemiya's article (1977), as corrected, shows that in the nonlinear case
FIML is asymptotically more efficient than 3SI^S under the assumption of normality. In
the linear case FIML is consistent even if the error terms are not normally distributed,
where "FIML" means the full information maximum liketihood estimator derived under
the assumption of normality. In the nonlinear case this is not in general true, although it
sometimes is. Phillips (1982) presents an example of a non linear model for which FIML
is consistent for a wide class of error distributions.
>
