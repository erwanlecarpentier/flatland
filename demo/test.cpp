
/**
 * OLTA paper
 */
void test(char * n) {
    std::vector<double> mp_range = {.0, .05, .1, .15, .2, .25, .3, .35, .4, .45, .5};
    std::string name(n);
    unsigned nbsim = 100;
    for(auto &mp : mp_range) { // for every misstep probability
        std::string cfg_path = "config/backup/discrete/" + name + ".cfg";
        std::string bkp_path = "data/" + name + std::to_string((int)(mp*100.)) + ".csv";
        std::cout << "Output: " << bkp_path << std::endl;
        parameters p(cfg_path.c_str());
        p.MISSTEP_PROBABILITY = mp;
        p.MODEL_MISSTEP_PROBABILITY = mp;
        run(nbsim,p,bkp_path.c_str(),false,true);
    }
}
