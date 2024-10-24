import { HttpClient } from "@angular/common/http";
import { inject, Injectable } from "@angular/core";
import { MoveCommand, RunCommand, Status } from "./model";

const API = "http://192.168.0.119";

@Injectable({ providedIn: 'root' })
export class Remote {
  #httpClient = inject(HttpClient);

  status() {
    return this.#httpClient.get<Status>(`${API}/status`);
  }

  move(data: MoveCommand) {
    return this.#httpClient.post(`${API}/move`, data);
  }


  setDiag(threshold: number) {
    return this.#httpClient.post(`${API}/diag`, {threshold});
  }

  run(data: RunCommand) {
    return this.#httpClient.post(`${API}/run`, data);
  }

}
