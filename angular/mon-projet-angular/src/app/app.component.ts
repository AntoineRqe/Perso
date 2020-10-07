import { Component, OnDestroy, OnInit } from '@angular/core';
import { rejects } from 'assert';
import { AppareilService } from './services/appareil.service';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/observable/interval';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit, OnDestroy{

  seconds: number;
  counterSubscription: Subscription;

  constructor() {}

  ngOnInit() {
    const counter = Observable.interval(1000);
    this.counterSubscription = counter.subscribe(
      (value) => {
        this.seconds = value;
      },
      (error) => {
        console.log("an error occured! : " + error);
      },
      () => {
        console.log("Observable completed!");
      }
    )
  };

  ngOnDestroy() {
    this.counterSubscription.unsubscribe();
  }

}
