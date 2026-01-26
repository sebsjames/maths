#include <iostream>
#include <sm/mat>

int main()
{
    int rtn = 0;

    sm::vec<float> rp = {};
    sm::vec<float> rn = sm::vec<float>::ux();
    sm::vec<float> v1 = { 0.5f, 0.0f, 0.5f };

    rp = {0.0f, 0.0f, 0.0f};
    std::cout << v1 << " reflected about point " << rp << " normal " << rn << " is " << (sm::mat<float, 4>::reflection (rp, rn) * v1) << std::endl;
    if ((sm::mat<float, 4>::reflection (rp, rn) * v1).less_one_dim() != sm::vec<float>{ -0.5f, 0.0f, 0.5f }) {
        --rtn;
    }

    rp = {0.25f, 0.0f, 0.0f};
    std::cout << v1 << " reflected about point " << rp << " normal " << rn << " is " << (sm::mat<float, 4>::reflection (rp, rn) * v1) << std::endl;
    if ((sm::mat<float, 4>::reflection (rp, rn) * v1).less_one_dim() != sm::vec<float>{ 0.0f, 0.0f, 0.5f }) {
        --rtn;
    }

    rp = {-0.25f, 0.0f, 0.0f};
    std::cout << v1 << " reflected about point " << rp << " normal " << rn << " is " << (sm::mat<float, 4>::reflection (rp, rn) * v1) << std::endl;
    if ((sm::mat<float, 4>::reflection (rp, rn) * v1).less_one_dim() != sm::vec<float>{ -1.0f, 0.0f, 0.5f }) {
        --rtn;
    }

    return rtn;
}
