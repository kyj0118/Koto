void mk_macfiles(){
  for (int i = 0; i <= 8; i++){
    int deg = i*5;
    double theta = (double) deg;
    theta = theta/180.0*TMath::Pi();
    double phi = 0;

    double dx = sin(theta)*cos(phi);
    double dy = sin(theta)*sin(phi);
    double dz = cos(theta);

    auto fname = Form("run_gamma1GeV_%ddeg.mac",deg);
    ofstream ofile(fname,ofstream::trunc);
    ofile << "/tracking/verbose -1" << endl;
    ofile << "/gps/position  0 0 0"<< endl;
    ofile << "/gps/direction " << dx << " " << dy << " "<< dz << endl;
    ofile << "/gps/particle  gamma" << endl;
    ofile << "/gps/energy    1000 MeV" << endl;
    ofile << "/run/beamOn    5000" << endl;
    ofile << "/vis/verbose -1" << endl;
    ofile.close();
  }
}
