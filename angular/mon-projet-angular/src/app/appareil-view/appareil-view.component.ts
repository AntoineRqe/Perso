import { Component, OnDestroy, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { AppareilService } from '../services/appareil.service';

@Component({
  selector: 'app-appareil-view',
  templateUrl: './appareil-view.component.html',
  styleUrls: ['./appareil-view.component.scss']
})
export class AppareilViewComponent implements OnInit, OnDestroy {

  appareil:any[];
  appareilSubscription: Subscription;

  isAuth = true;

  lastUpdate = new Promise((resolve, reject) => {
    const date = new Date();
    setTimeout(
      () => {
        resolve(date);
      }, 2000
    );
  });

  appareils: any[];

  constructor(private appareilService: AppareilService) {}

  ngOnInit() {
    this.appareilSubscription = this.appareilService.appareilSubject.subscribe(
      (appareils: any[]) => {
        this.appareils = appareils;
      }
    )
    this.appareilService.emitAppareilSubject();
  }

  onSwitchOn() {
    this.appareilService.switchOnAll();
  }

  onSwitchOff() {
    if(confirm("Are you sure?")) {
      this.appareilService.switchOffAll();
    } else {
      return null;
    } 
  }

  ngOnDestroy()
  {
    this.appareilSubscription.unsubscribe();
  }

  onSave()
  {
    this.appareilService.saveAppareilsToServer();
  }

  onFetch()
  {
    this.appareilService.getAppareilsFromServer();
  }
}
