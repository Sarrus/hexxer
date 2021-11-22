__global__
void solver()
{

}

extern "C" void solveWithCUDA()
{
    solver<<<1, 1>>>();
}