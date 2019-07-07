double power(int b);

__kernel void helloworld(int SCREEN_WIDTH, int nZoom, double x, double y, __global double* out)
{
	int num = get_global_id(0);
	double cr = (((num - 1) % SCREEN_WIDTH) - SCREEN_WIDTH / 2.0) / (SCREEN_WIDTH * power(nZoom - 1)) + x;
	double ci = (((num - 1) / SCREEN_WIDTH) - SCREEN_WIDTH / 2.0) / (SCREEN_WIDTH * power(nZoom - 1)) + y;

	double zr = 0.0;
	double zi = 0.0;

	int nIter = 0;

	while (zr * zr + zi * zi < 4 && nIter <= 1000) {
		double tr = zr * zr - zi * zi + cr;
		zi = 2 * zr *zi + ci;
		zr = tr;
		nIter++;
	}

	if (nIter <= 1000) {
		out[num] = (int)((255 * nIter) / 1000);
	}
	else {
		out[num] = 0;
	}
}

double power(int b){
	
	if(b < 0){
		int a = 0.5;
	
		for(int i = -1; i > b; i--){
			a = a * 0.5;
		}
		return a;
	} else if(b == 0){
		return 1;
	}else {
		int a = 2;
	
		for(int i = 1; i < b; i++){
			a = a * 2;
		}
		return a;
	}
}