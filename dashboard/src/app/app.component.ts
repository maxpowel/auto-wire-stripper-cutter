import { Component, inject, signal } from '@angular/core';
import { FormControl, FormGroup, FormsModule, ReactiveFormsModule, Validators } from '@angular/forms';
import { RouterOutlet } from '@angular/router';
import { HotToastService } from '@ngxpert/hot-toast';
import { injectMutation, injectQuery } from '@tanstack/angular-query-experimental';
import { Remote } from './data/remote';
import { lastValueFrom, tap } from 'rxjs';
import { MoveCommand, RunCommand, SaveThreshold } from './data/model';
import { NgClass } from '@angular/common';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, FormsModule, ReactiveFormsModule, NgClass],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  toast = inject(HotToastService)
  #remote = inject(Remote)

  running = signal(false)

  stepsMultiplier = 1000
  stepSize = 1

  
  wireForm = new FormGroup({
    pre: new FormControl(0, Validators.required),
    length: new FormControl(0, Validators.required),
    post: new FormControl(0, Validators.required),
    repetitions: new FormControl(0, Validators.required),
    gauge: new FormControl(0, Validators.required),
  });

  thresholdForm = new FormGroup({
    threshold: new FormControl(0, Validators.required),

  });

  status$ = injectQuery(() => ({
    queryKey: ['status'],
    queryFn: () => lastValueFrom(this.#remote.status().pipe(tap(resp => {
      this.wireForm.patchValue({
        pre: resp.comps[0],
        length: resp.comps[1],
        post: resp.comps[2],
        gauge: resp.gauge,
        repetitions: resp.repetitions
      })

      this.thresholdForm.patchValue({
        threshold: resp.threshold
      })
      //resp.comps
    }),
      this.toast.observe({
        loading: 'Checking status...',
        success: 'Status Ok!',
        error: 'Could not connect to device',
      })
    )),
  }))

  values = this.status$.data();

  moveMutation$ = injectMutation((client) => ({
    mutationFn: (data: MoveCommand) =>
      lastValueFrom(
        this.#remote.move(data).pipe(
          this.toast.observe({
            loading: 'Sending',
            success: 'Ok!',
            error: 'Could not send command',
          })
        )
      )
  }));

  runMutation$ = injectMutation((client) => ({
    mutationFn: (data: RunCommand) =>
      lastValueFrom(
        this.#remote.run(data).pipe(
          this.toast.observe({
            loading: 'Sending',
            success: 'Ok!',
            error: 'Could not send command',
          })
        )
      )
  }));

  thresholdMutation$ = injectMutation((client) => ({
    mutationFn: (data: SaveThreshold) =>
      lastValueFrom(
        this.#remote.setDiag(data.threshold).pipe(
          this.toast.observe({
            loading: 'Sending',
            success: 'Ok!',
            error: 'Could not send command',
          })
        )
      )
  }));

  onSubmit() {

  }

  up() {
    this.moveMutation$.mutate({
      lin: this.stepSize,
      wire: undefined,
      linStep: undefined,
      wireStep: undefined
    })
  }

  down() {
    this.moveMutation$.mutate({
      lin: -this.stepSize,
      wire: undefined,
      linStep: undefined,
      wireStep: undefined
    })
  }

  extrude() {
    this.moveMutation$.mutate({
      lin: undefined,
      wire: this.stepSize,
      linStep: undefined,
      wireStep: undefined
    })
  }

  retract() {
    this.moveMutation$.mutate({
      lin: undefined,
      wire: -this.stepSize,
      linStep: undefined,
      wireStep: undefined
    })
  }

  upSteps() {
    this.moveMutation$.mutate({
      lin: undefined,
      wire: undefined,
      linStep: this.stepsMultiplier,
      wireStep: undefined
    })
  }

  downSteps() {
    this.moveMutation$.mutate({
      lin: undefined,
      wire: undefined,
      linStep: -this.stepsMultiplier,
      wireStep: undefined
    })

  }

  extrudeSteps() {
    this.moveMutation$.mutate({
      lin: undefined,
      wire: undefined,
      linStep: undefined,
      wireStep: this.stepsMultiplier,
    })
  }

  retractSteps() {
    
    this.moveMutation$.mutate({
      lin: undefined,
      wire: undefined,
      linStep: undefined,
      wireStep: -this.stepsMultiplier,
    })
  }

  run() {
    if (this.wireForm.valid) {
      let values = this.wireForm.value
      this.runMutation$.mutate({
        pre: values.pre??0,
        len: values.length??0,
        pos: values.post??0,
        repetitions: values.repetitions??0,
        gauge: values.gauge??0
      })
    }
  }

  saveThreshold() {
    this.thresholdMutation$.mutate({
      threshold: this.thresholdForm.value.threshold??255,
    })
  }
}
