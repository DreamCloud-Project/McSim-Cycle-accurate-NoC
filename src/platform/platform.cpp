#include "platform.h"

Platform::Platform(sc_module_name name) :
		sc_module(name) {

	// Create the tiles making the platform
	for (int x = 0; x < CommonParameter::dim_x; x++) {
		for (int y = 0; y < CommonParameter::dim_y; y++) {

			// Create tile
			string label = "Tile[" + int_to_str(x) + "][" + int_to_str(y) + "]";
			tile[x][y] = new Tile(label.data(), x, y);
			tile[x][y]->proc->initialize(x, y);
			tile[x][y]->router->initialize(x, y);
			tile[x][y]->initialize();
			tile[x][y]->clk(clk);
			tile[x][y]->reset(reset);

			// Bind all interconnect signals
			for (int i = 0; i < ROUTER_PORTS - 1; i++) {
				switch (i) {
				case (WEST):
					tile[x][y]->valid_in[i](W_valid_in[x][y]);
					tile[x][y]->flit_in[i](W_flit_in[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								W_in_vc_buffer_rd[x][y][k]);
					}

					tile[x][y]->valid_out[i](W_valid_out[x][y]);
					tile[x][y]->flit_out[i](W_flit_out[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								W_out_vc_buffer_rd[x][y][k]);
					}
					break;
				case (EAST):
					tile[x][y]->valid_in[i](W_valid_out[x + 1][y]);
					tile[x][y]->flit_in[i](W_flit_out[x + 1][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								W_out_vc_buffer_rd[x + 1][y][k]);
					}

					tile[x][y]->valid_out[i](W_valid_in[x + 1][y]);
					tile[x][y]->flit_out[i](W_flit_in[x + 1][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								W_in_vc_buffer_rd[x + 1][y][k]);
					}
					break;
				case (NORTH):
					tile[x][y]->valid_in[i](N_valid_in[x][y]);
					tile[x][y]->flit_in[i](N_flit_in[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								N_in_vc_buffer_rd[x][y][k]);
					}

					tile[x][y]->valid_out[i](N_valid_out[x][y]);
					tile[x][y]->flit_out[i](N_flit_out[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								N_out_vc_buffer_rd[x][y][k]);
					}
					break;
				case (SOUTH):
					tile[x][y]->valid_in[i](N_valid_out[x][y + 1]);
					tile[x][y]->flit_in[i](N_flit_out[x][y + 1]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								N_out_vc_buffer_rd[x][y + 1][k]);
					}

					tile[x][y]->valid_out[i](N_valid_in[x][y + 1]);
					tile[x][y]->flit_out[i](N_flit_in[x][y + 1]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								N_in_vc_buffer_rd[x][y + 1][k]);
					}
					break;
				default:
					;
				}
			}
		}
	} // End platform creation
}
